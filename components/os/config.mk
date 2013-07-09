check-os-path:	check-tiles-path

report-os-path:	check-os-path
	@echo OS: Cross toolchain prefix in use: $(ISHW_CROSS_TOOLCHAIN_PREFIX)
