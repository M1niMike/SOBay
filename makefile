#all
all:
	gcc backend.c backend.h -o backend
	gcc frontend.c frontend.h -o frontend

#frontend
frontend:
	gcc frontend.c frontend.h -o frontend

#backend
backend:
	gcc backend.c backend.h -o backend

#remove all
rmnall:
	rm backend	
	rm frontend

#remove only backend
rmbackend:
	rm backend	

#remove only frontend	
rmfrontend:
	rm frontend	
	


