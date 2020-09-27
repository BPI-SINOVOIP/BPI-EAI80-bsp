    SECTION_CODE_RUN_RAM_PROLOGUE(_CODE_RAM_RUN_SECTION_NAME,,)
	{
		__code_run_ram_start = .;
		KEEP(*(".code_run_ram*"))
		__code_run_ram_end = .;
	} GROUP_CODE_RUN_RAM_LINK_IN(RAMABLE_REGION, ROMABLE_REGION)
