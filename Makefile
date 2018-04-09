ssu_proc : ssu_proc.c option_v.c option_l.c option_f.c option_s.c option_e.c
	gcc -o ssu_proc ssu_proc.c option_v.c option_l.c option_f.c option_s.c option_e.c
clean :
	rm ssu_proc
