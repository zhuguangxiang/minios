
void panic(const char *);
void __div0(void)
{
	panic("divide by zero");
}

