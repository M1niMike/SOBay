
all:
	gcc backend.c backend.h users_lib.o -o backend -pthread
	gcc frontend.c frontend.h -o frontend -pthread


frontend:
	gcc frontend.c frontend.h -o frontend -pthread


backend:
	gcc backend.c backend.h users_lib.o -o backend -pthread

rmfrontend:
	rm frontend

rmbackend:
	rm backend

clean:
	rm backend	
	rm frontend


	


