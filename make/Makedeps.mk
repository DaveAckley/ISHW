
# We need a kickstart rule to get .d files going, before their first real compile-time generation:
# Pattern rule for d files
$(OBJDIR)/%.d:	
	mkdir -p $(OBJDIR)
	-touch $@

# After we make .d's, let's keep them
.PRECIOUS:	$(OBJDIR)/%.d

# Suck in generated autodependencies
AUTODEPS:=$(wildcard $(OBJDIR)/*.d)
-include $(AUTODEPS)
