AS = genim2
CC = sc
CFLAGS = ERRORREXX
LD_LIBS=lib:scm.lib lib:sc.lib lib:amiga.lib 


Amiglobe:       main.o editeur.o map_function.o database.o 3d.o map_divers.o divers.o AmiGlobe_Interf.o iff/jiff.o flag.o iff_fct.o read_bin.o read_org.o chemin.o projection.o fichier.o
        SLINK lib:c.o main.o editeur.o map_function.o database.o 3d.o divers.o map_divers.o AmiGlobe_Interf.o iff/jiff.o flag.o iff_fct.o read_bin.o read_org.o chemin.o projection.o fichier.o LIBRARY $(LD_LIBS)  ADDSYM TO Amiglobe

main.o: main.c  map_function_protos.h amiglobe_types.h database_protos.h windowdef.h amiglobe_interf.h

map_function.o: map_function.c  main_protos.h amiglobe_types.h

divers.o: divers.c divers.h amiglobe_interf.h

map_divers.o: map_divers.c amiglobe_types.h

editeur.o:      editeur.c map_function_protos.h main_protos.h amiglobe_types.h

iff_fct.o:      iff_fct.c

database.o:     database.c amiglobe_interf.h

read_bin.o:     read_bin.c read_bin.h amiglobe_types.h map_function_protos.h map_function_types.h

read_org.o:     read_org.c read_org.h amiglobe_interf.h

chemin.o:       chemin.c chemin.h

projection.o:   projection.c projection.h

AmiGlobe_Interf.o: AmiGlobe_Interf.c AmiGlobe_Interf.h

fichier.o:  fichier.c fichier.h

test_flag:      test_flag.o flag.o 3d.o
        SLINK lib:c.o test_flag.o flag.o 3d.o LIBRARY $(LD_LIBS) ADDSYM 
                                TO test_flag
                
new_lac.pt_bin_small:   new_lac.pt_bin create_tab_bin
                create_tab_bin  new_lac.pt_bin new_lac.pt_bin_small
                                
create_tab_bin: create_tab_bin.c
            sc LINK create_tab_bin 
.c.o:
        $(CC) $< $(CFLAGS)
.s.o:
        $(AS) -L -o $@ $< 
