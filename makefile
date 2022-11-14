
all:
	gcc backend.c backend.h users_lib.o -o backend
	gcc frontend.c frontend.h -o frontend


frontend:
	gcc frontend.c frontend.h -o frontend


backend:
	gcc backend.c backend.h users_lib.o -o backend


rmall:
	rm backend	
	rm frontend


rmbackend:
	rm backend	

rmfrontend:
	rm frontend	
	


