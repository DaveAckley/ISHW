ISHW_TARGETS_HELP+="\nMFM TARGETS\n"
ISHW_TARGETS_HELP+="make mfm\n\tbuild the basic MFM execution environment\n"

check-mfm-path:	check-os-path

report-mfm-path:	check-mfm-path
	@echo MFM:

mfm:	FORCE
	@echo "Target $@ not yet implemented"
	@exit 99
