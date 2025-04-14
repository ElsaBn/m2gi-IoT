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
- Pourquoi j'ai l'erreur indiquée dans la section lancement ? Test avec utilisation du script linker du cours : fonctionne. -> Peut-être problème version compilateur (à regarder).
- Est-ce que pulse audio est un problème/faut-il le désactiver (comment) ? -> Pas de problème.
- Est-ce que le comportement obtenu (écriture des caractères fait continuer sur la ligne et à l'entrée reviens au début de la ligne) est correct ? -> à priori oui.

###### Notes :
Sur mandelbrot : utilisation script linker diapo 17.

Commandes : 
- make debug 
- gdb build/kernel.elf (autre terminal)
	- tar rem :1234
	- br _start
	
Sur mandelbrot faire : gdb-multiarch -ex "target remote localhost:1234" build/kernel.elf



## Semaine 2



### Nouveaux fichiers :
[ISR : Interrupt Service Routine (gère les interruptions)]
- *irq.s* : fichier assembleur pour la gestions des registres lors d'une interruption.
- *isr.h* : header de *isr.c*, contient les signatures de fonctions concernant le VIC (Vectored Interrupt Controller)(setup, (dés)activation, chargement), celles concernant le coeur ((dés)activation manière globale) et des macro concernant les timers et UART (adresses et masques pour les interruptions).
- *isr.c* : contient le corps des fonctions définies dans le .h et des appels à des fonctions externes définies dans le fichier assembleur. Un handler pour les callback est aussi présent.
- *isr-mmio.h* : contient l'adresse de base pour le VIC et les offset de différents registres du VIC.

### Complétion :
Pour les adresses, offsets, masques : trouvables dans les docs correspondantes (souvent indiquées dans les fichiers). Attention, certaines docs peuvent se contredire (exemple VIC_BASE_ADDR) : se référer à celle de la carte qu'on manipule en priorité (en opposition à celle du composant car les fabriquants n'ont pas la vue sur comment ça sera utilisé).

- *isr-mmio.h* : compléter adresse et offset.
- *isr.h* : compléter les adresses et masques et définir le nombre maximum d'interruptions géré par le VIC (32 car la boucle de isr() était de 32 pour parcourir le handler).
- *isr.c* : complétion des fonctions :
	- isr() : reprise de l'exemple du cours : création fonction pour récupérer la liste des interruptions (load), pour vic_ack je ne sais pas par quoi remplacer (**note** : le cours indique "If you do not acknowledged, the device will never raise its interrupt again" -> comment ack ? => pas bosin d'ack ici).
	- setup : il faut initialiser le hardware et le software donc utilisation de _irqs_setup() pour le 1er et pour le 2nd je ne sais pas.
	- enable : écriture sur le registre VICINTENABLE de l'activation de l'interruption en utilisant un masque pour décaler et changer le bon bit. Un handler est passé en paramètre mais je ne comprends pas pourquoi
	- disable : écriture sur le registre VICINTCLEAR en passant un 1 car sinon ne clear pas le bit (cf isr-mmio).

- *main.c* : fonction start() : réutilisation jusqu'à uart_enable puis reprise de la version mise à jour du cours. Initalisation du vic, activation (**note**: que mettre comme paramètre de la fct enable (car pas de boutons et de leds) ? ), et activation des interruptions au niveau hardware avec core_enable. Boucle infinie avec core_halt()

- *Makefile* : ajout irq.o et isr.o aux objs pour qu'il les ajoute lors de la compilation

- *kernel.ld* : ajout build/irq.o(.text)

Make avec ce qui a été completé : 1 erreur :
*arm-none-eabi-ld: build/irq.o: in function '_irqs_disable': irq.s:56: undefined reference to `irq_stack_top'*

Besoin ajout à la fin du kernel.ld. En relisant startup.s j'ai vu qu'il fallait enlever le set de CPSR_IRQ_FLAG pour autoriser les interruptions. ==> Make ok

Essai de lancement avec gdb : n'arrive plus à se connecter (sûrement lié au -S cf : https://github.com/ilg-archived/qemu/issues/70 => ça fonctionnait avant (pourtant version 3.1.0 actuellement)). Evidemment si make run normal pas possible d'écrire puisque j'ai enlevé send/receive.

**Note** : comment tester les interruptions ici en réalité ?

###### Notes :
Globalement pas compris ce qu'il fallait vraiment faire (cf par ex. le point sur isr).

Quelle est l'utilisation des macro/masques UART et TIMER définit ? (Ici/dans le cours il n'y avait pas l'air d'en avoir besoin )



## Semaine 3



### But :
Bien séparer la partie interruption du déroulement du programme normal (fonction *_start*).
Lors d'une interruption, s'il y a des données qui sont envoyées pendant ce temps, les stocker dans un buffer circulaire. Lors de la reprise, la fonction read_listener indique qu'il y a des données disponibles. Le listener de write sera appelé lorsqu'il y a de la place pour écrire des octets, mais si une écriture à échouée précédemment parce qu'il n'y en avait plus.

Il faut compléter les fonctions uart_init, _read et _write. La fonction *_init* prendra en paramètre des listeners callback read et write, le numéro de l'uart qui l'appelle et le cookie (buffer), et *_read/_write* renvoient un booléen quand ils ont accompli leur fonction.

### Actuellement :

Difficile de comprendre vraiment comment faire les choses. Récupération du code du cours :
- *ring-buffer.c/.h* : contient le buffer circulaire pour stocker les données
- structure *cookie* (main.h) : qui sert probablement à stocker les informations du buffer.
- fonctions read_listener/writer_listener/write_amap (*main.c*): les listener servent pour lire/écrire dans/depuis le buffer, tant qu'il y a la place. Write_amap écrit dans le shell.
- retour de la fonction uart_receive (passage de void à uint_8)

### Ensuite :
- Quelle est cette fonction shell ?
- **Implémentation des fonctions uart nécessaires**.
- La fonction beep n'est pas "trouvée" (*implicit declaration of function 'beep'*) mais quand passage de la souris dessus me donne ses informations. -> la commenter



## Semaine 4

Après le cours précédent beaucoup n'ont pas réussi -> continuer sur ce qu'on faisait à la base.

Explication de l'utilisation des listeners (~cf semaine précédente)

-> Idéal à la fin = programmation évenementiel.

### Complétion dans step3

Fin de interruptions :
- *isr.c* :
	- fonction isr : il faut désactiver les interruptions durant le traitement et les réactiver à la fin.
	- vic_enable_irq : ajout du callback et cookie dans le handler.
	- vic_setup_irqs : remise à 0 des interruptions.

=> Je pensais pouvoir écrire dans le terminal comme au step 1 mais ne fonctionne toujours pas (ça fonctionnait chez les autres ?)

Difficile de comprendre ce que je dois faire et je ne veux pas juste copier bêtement le code des autres. Sans exemple plus précis du cours ou une vraie solution je ne saurais pas faire.








