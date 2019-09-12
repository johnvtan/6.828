# Exercise 1
## Question 1
Nothing else needs to be done since the registers for each environment are stored separately. When the trapframe is
popped and the registers are reloaded, the EFLAGS register will contain the appropriate IOPL bits set and so only the
environments that were granted access to IO by th kernel are able to access the IO devices.
