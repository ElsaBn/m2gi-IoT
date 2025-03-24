# m2gi-IoT

## Semaine 1 
- Pas d'ordinateur personnel
- Besoin d'installer arm / qemu : demande au service IT
	
### Makefile :
Différentes parties : 
##### Partie configurable pour choisir : 
- le type de carte à programmer : *versatile* (carte ARM),
- le CPU : *cortex-a8* (microprocesseur multi-coeur 32 bits, sorti en 2006),
- les outils de compilation utilisé : *arm-none-eabi* (pour le développement embarqué bare-metal sur ARM),
- si le débug est activé : le point d'interrogation indique la valeur à mettre si elle n'a pas déjà été précisée (ici activé), 
- le dossier dans lequel on veut mettre les objets compilés (*build/*), 
- et les fichiers objets (.o) : *startup* (programme au démarrage), *main* (programme qu'on lance), *exception* (contient les handler d'exception), *uart* (emetteur/receveur pour la serial line)
##### Partie non configurable :
- OBJS permet d'ajouter le préfixe du dossier de build aux objets.
- Si la carte est versatile d'autres paramètres sont spécifiés : 
	- *nographic* : pour ne pas avoir de support graphique (seulement CLI)
	- *serial mon:stdio* : redirige le port vers la sortie stdio
	- taille de la mémoire en kilo octets (32 + K)
	- type de machine : versatile ab (celle utilisée pour ce sours)
	- l'executable QEMU utilisé (*qemu-system-arm*)
	- les argumets QEMU : concaténation d'arguments définis plus haut
	- options du compilateur : *-c* (compiler sans lier), *-mcpu* (spécifie le nom du cpu), *-nostdlib* (ne pas utiliser les bibliothèques et fichiers standards), *-ffreestanding* (environnement où les bibliothèque standard n'existe pas), *-DCPU* (définit le cpu), *-DMEMORY* (définit la taille de mémoire utilisée en octets) 
	- options de l'assembleur : *-mcpu* (spécifie le nom du cpu)
	- options de l'éditeur de lien : *-T* (script de l'editeur de lien), *-nostdlib* (indique l'utilisation de bibliothèques spécifiées dans la commande, les autres ignorées), *-static* (ne crée pas de lien dynamique avec les bibliothèques partagées)
- Si le debug est activé :
	- ajoute des options pour activer le debug et la comptabilité avec
- Si la machine n'a pas été spécifiée alors il y a un message d'erreur indiquant le besoin de choisir une carte

##### Règles de compilation :
- Compilation des fichiers .c en .o avec les options de compilation
- Compilation des fichiers assembleur .s en .o avec les options d'assembleur

##### Construction et lien :
- *all* : regroupe toutes les cibles de compilation ci dessous (excepté clean)
- *$(BUILD)kernel.elf* : construction du fichier kernel.elf (éditions de lien), avec les fichiers objets et les options définies plus haut.
- *$(BUILD)kernel.bin* : construction du fichier kernel.bin (binaire), avec le fichier elf construit. L'outil de compilation utilise objcopy pour copier les données, et indique que la sortie est un binaire.
- *build* permet de créer le dossier de build
- *clean* supprime ce dossier

##### Lancement de QEMU :
Cette partie ne fonctionne que dans le cas d'une carte versatile.
La cible *run* utilise la cible all et lance QEMU avec les arguments spécifiés plus haut, et le fichier kernel.elf. La cible *debug* y ajoute une connexion au débugger gdb sur le port 1234 et -S qui permet de ne pas démarrer le CPU au démarrage et nécessite de taper "c".

### Fichiers de code :
Les fichiers .s sont les fichiers écrits en assembleur, pour le startup et la gestion des exceptions.

Les fichiers .h sont les entêtes des fichiers .c correspondants. Ils définissent des constantes, des signatures de fonctions, et pour *main.h* des fonctions inline. Ces fonctions permettent de lire et d'écrire dans des registres MMIO de différentes tailles (8, 16...) et de les setter ou les vider. *uart_mmio.h* n'a pas de fichier .c correspondant, il est appelé par d'autres et définis les constantes d'adresse concernant la serial line UART.

Les fichiers .c :
- *uart.c* : contient les fonctions d'initialisation et d'activation/désactivation d'UART, mais aussi les fonctions permettant de recevoir et d'envoyer des caractères
- *main.c* : dans ce fichier on retrouve la fonction check_stacks, qui permet de vérifier que l'adresse du haut de la pile ne dépasse pas la mémoire allouée (si c'est le cas utilisation de panic() qui bloque le programme dans une boucle infinie), et la fonction _start qui initialise UART et boucle indéfiniment en utilisation les fonctions de réception et d'envoi de UART.

### Lancement :
Sans rien modifier essai avec *make debug* (**note**: ne fonctionne pas à distance sur mandelbrot, erreur section .bss (100 à 117) qui overlap la section .text (000 à 307)). Tout compile bien (**note**: erreurs de pulse audio mais ne semble pas affecter). Est en attente.

Utilisation de gdb (autre terminal) sur le fichier kernel.elf. Connexion à la cible distante sur le port 1234 (spécifié dans la configuration) avec la commande tar[get] rem[ote] [localhost]:1234 dans gdb et faire des continue/next (**note**: encore une fois problème sur mandelbrot, *warning: while parsing target description (at line 1): Target description specified unknown architecture "arm"* et en cherchant sur internet j'ai trouvé une autre commande cf section notes).

But : faire fonctionner UART10 et une console echo.

### Complétion :
Dans *uart-mmio.h* il faut compléter les adresses de base et les registres de data et de flag. Cf la documentation UART pour les registres et versatile pour les adresses.

Dans *uart.c* il faut compléter les fonctions send et receive. Adaptation ce qu'il y a dans le cours pour les uart_read/write et ajout de masque dans le header mmio. Le registre channel status (SR) du cours correspond au registre de flag (FR) et celui FIFO à celui de data (DR).
Après ça je peux écrire dans le terminal où le make à été lancé. On peut suivre l'avancement du programme avec gdb et voir que tant qu'on écrit pas de caractère on est bloqué dans uart_receive et à l'écriture on passe à uart_send et on revient à receive (boucle infinie).



###### Questions à poser :
- Pourquoi j'ai l'erreur indiquée dans la section lancement ? Test avec utilisation du script linker du cours : fonctionne.
- Est-ce que pulse audio est un problème/faut-il le désactiver (comment) ?
- Est-ce que le comportement obtenu (écriture des caractères fait continuer sur la ligne et à l'entrée reviens au début de la ligne) est correct ?

###### Notes :
Sur mandelbrot : utilisation script linker diapo 17.

Commandes : 
- make debug 
- gdb build/kernel.elf
	- tar rem :1234
	- br _start
	
Sur mandelbrot faire : gdb-multiarch -ex "target remote localhost:1234" build/kernel.elf
	






