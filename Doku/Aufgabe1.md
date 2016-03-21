# Praktikum 1

## Aufgabe 1.2

### Create
#### Joinable

Wenn ein Thread mit PTHREAD_CREATE_JOINABLE aufgerufen wird, dann kann sich ein anderer Thread mit diesem verbinden.
Neu erstelle Threads sind standardmäßig joinable.

#### Detachable

Wenn ein Thread mit PTHREAD_CREATE_DETACHED aufgerufen wird, dann werden nach der Terminierung sämtliche
Resourcen freigegeben. Ein joinable Thread kann später über die Methode pThread_detach() detached werden.

### Join

Blockt den aufrufenden Thread, bis der angegebene Thread terminiert. Z.B. ruft Thread 1 auf Thread 2
pthread_join() auf, sodass Thread 1 blockiert bis Thread 2 terminiert.

### Exit
Der aufrufende Thread wird terminiert und alle über join verbundene Threads lösen sich aus der Blockierung.

## Aufgabe 1.3

Siehe Aufgabe1.c

