#!/usr/bin/env python3
import sys
import subprocess
import re
import os
import xml.etree.ElementTree as ET
from email.utils import parseaddr
import sh
import logging
import argparse
#from check_identity import verify_signed_off

if "UGELIS_BASE" not in os.environ:
    logging.error("$UGELIS_BASE environment variable undefined.\n")
    exit(1)

logger = None
DOCS_WARNING_FILE = "doc.warnings"

repository_path = os.environ['UGELIS_BASE']
sh_special_args = {
    '_tty_out': False,
    '_cwd': repository_path
}

# list_undef_kconfig_refs.py makes use of Kconfiglib
sys.path.append(os.path.join(repository_path, "scripts/kconfig"))
import list_undef_kconfig_refs


def init_logs():
    global logger
    log_lev = os.environ.get('LOG_LEVEL', None)
    level = logging.INFO
    if log_lev == "DEBUG":
        level = logging.DEBUG
    elif log_lev == "ERROR":
        level = logging.ERROR

    console = logging.StreamHandler()
    format = logging.Formatter('%(levelname)-8s: %(message)s')
    console.setFormatter(format)
    logger = logging.getLogger('')
    logger.addHandler(console)
    logger.setLevel(level)

    logging.debug("Log init completed")

def parse_args():
    parser = argparse.ArgumentParser(
                description="Check for coding style and documentation warnings.")
    parser.add_argument('-c', '--commits', default=None,
            help="Commit range in the form: a..b")
    return parser.parse_args()

def get_shas(refspec):

    sha_list = sh.git("rev-list",
        '--max-count={0}'.format(-1 if "." in refspec else 1),
        refspec, **sh_special_args).split()

    return sha_list



def run_gitlint(tc, commit_range):
    proc = subprocess.Popen('gitlint --commits %s' %(commit_range),
                shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    msg = ""
    if proc.wait() != 0:
        msg = proc.stdout.read()

    if msg != "":
        failure = ET.SubElement(tc, 'failure', type="failure", message="commit message error on range: %s" %commit_range)
        failure.text = (msg.decode('utf8'))
        return 1

    return 0


def run_checkpatch(tc, commit_range):
    output = None
    out = ""

    diff = subprocess.Popen(('git', 'diff', '%s' %(commit_range)), stdout=subprocess.PIPE)
    try:
        output = subprocess.check_output(('%s/scripts/checkpatch.pl' %repository_path,
            '--mailback', '--no-tree', '-'), stdin=diff.stdout,
            stderr=subprocess.STDOUT, shell=True)

    except subprocess.CalledProcessError as ex:
        m = re.search("([1-9][0-9]*) errors,", ex.output.decode('utf8'))
        if m:
            failure = ET.SubElement(tc, 'failure', type="failure", message="checkpatch issues")
            failure.text = (ex.output.decode('utf8'))
            return 1

    return 0


def run_kconfig_undef_ref_check(tc, commit_range):
    # Parse the entire Kconfig tree, to make sure we see all symbols
    os.environ["ENV_VAR_BOARD_DIR"] = "boards/*/*"
    os.environ["ENV_VAR_ARCH"] = "*"

    # Hack:
    #
    # When using 'srctree', Kconfiglib checks the current directory before
    # checking the 'srctree' directory (only for compatibility with the C
    # tools... this behavior is pretty bad for Kconfig files).
    #
    # This can cause problems for external projects that happen to run
    # check-compliance.py from a directory that contains Kconfig files that
    # overlap with Kconfig in the uGelis tree (subsys/Kconfig would override
    # the same file in the base uGelis repo, etc.).
    #
    # Work around it by temporarily changing directory instead of using
    # 'srctree'.
    cur_dir = os.getcwd()
    os.chdir(repository_path)
    try:
        # Returns the empty string if there are no references to undefined symbols
        msg = list_undef_kconfig_refs.report()
        if msg:
            failure = ET.SubElement(tc, "failure", type="failure",
                                    message="undefined Kconfig symbols")
            failure.text = msg
            return 1

        return 0

    finally:
        # Restore working directory
        os.chdir(cur_dir)


def verify_signed_off(tc, commit):

    signed = []
    author = ""
    sha = ""
    parsed_addr = None
    for line in commit.split("\n"):
        match = re.search("^commit\s([^\s]*)", line)
        if match:
            sha = match.group(1)
        match = re.search("^Author:\s(.*)", line)
        if match:
            author = match.group(1)
            parsed_addr = parseaddr(author)
        match = re.search("signed-off-by:\s(.*)", line, re.IGNORECASE)
        if match:
            signed.append(match.group(1))

    error1 = "%s: author email (%s) needs to match one of the signed-off-by entries." %(sha, author)
    error2 = "%s: author email (%s) does not follow the syntax: First Last <email>." %(sha, author)
    error = 0
    failure = None
    if author not in signed:
        failure = ET.SubElement(tc, 'failure', type="failure", message="identity error")
        failure.text = error1
        error = 1
    if not parsed_addr or len(parsed_addr[0].split(" ")) < 2:
        if not failure:
            failure = ET.SubElement(tc, 'failure', type="failure", message="identity error")
            failure.text = error2
        else:
            failure.text = failure.text + "\n" + error2
        error = 1

    return error

def run_check_identity(tc, range):
    error = 0
    for f in get_shas(range):
        commit = sh.git("log","--decorate=short", "-n 1", f, **sh_special_args)
        error += verify_signed_off(tc, commit)

    return error


def check_doc(tc, range):

    if os.path.exists(DOCS_WARNING_FILE) and os.path.getsize(DOCS_WARNING_FILE) > 0:
        with open(DOCS_WARNING_FILE, "rb") as f:
            log = f.read()
            failure = ET.SubElement(tc, 'failure', type="failure",
                        message="documentation issues")
            failure.text = (log.decode('utf8'))
        return 1

    return 0



tests = {
        "gitlint": {
            "call": run_gitlint,
            "name": "Commit message style",
            },
        "identity": {
            "call": run_check_identity,
            "name": "Author Identity verification",
            },
        "checkpatch": {
            "call": run_checkpatch,
            "name": "Code style check using checkpatch",
            },
        "checkkconfig": {
            "call": run_kconfig_undef_ref_check,
            "name": "Check Kconfig files for references to undefined symbols",
            },
        "documentation": {
            "call": check_doc,
            "name": "New warnings and errors when building documentation",
            }
        }

def run_tests(range):
    run = "Commit Message / Documentation / Coding Style"
    eleTestsuite = None
    fails = 0
    passes = 0
    errors = 0
    total = 0
    filename = "compliance.xml"

    eleTestsuites = ET.Element('testsuites')
    eleTestsuite = ET.SubElement(eleTestsuites, 'testsuite', name=run, time="0",
            tests="%d" %(errors + passes + fails),  failures="%d" %fails,  errors="%d" %errors, skip="0")

    for test in tests.keys():

        total += 1
        eleTestcase = ET.SubElement(eleTestsuite, 'testcase', classname="%s"
                %(test), name="%s" %(tests[test]['name']), time="0")

        fails += tests[test]['call'](eleTestcase, range)

    eleTestsuite.set("tests", "%s" %total)
    eleTestsuite.set("failures",  "%s" %fails)

    result = ET.tostring(eleTestsuites)
    f = open(filename, 'wb')
    f.write(result)
    f.close()
    return fails


def main():
    args = parse_args()
    if not args.commits:
        exit(1)
    fails = run_tests(args.commits)
    print(fails)
    sys.exit(fails)

if __name__ == "__main__":
    #init_logs()
    main()

