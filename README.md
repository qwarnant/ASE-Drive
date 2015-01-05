ASE
===
Binôme : Quentin Warnant & Antoine Durigneux
Date : 03/01/14

Notre bibliothèque de gestion de disque dur est maintenant complète.
En effet, nous disposons de trois séries de programmes principaux :
- mvol est l'homologue de la commande linux fdisk, elle permet de créer, supprimer et lister des partitions sur un disque dur virtuel
- checkdisk est un programme utilitaire destiné à créer un disque en remplaçant un disque existant (mkhd), à débugger le disque (status, debug_fill, debug_free), à créer et gérer un nouveau file system sur une partition (mknfs)
- **file permettent de créer des fichiers sur un file system d'une partition

Pour passer la librairie en mode DEBUG, il est nécessaire de décommenter la ligne suivante dans le fichier tools.h :
#DEFINE DEBUG

ALIAS UTILITAIRES pour la librairie :
alias frmt='./checkdisk frmt'
alias mvol='./mvol'
alias mknfs='./checkdisk mknfs'
alias fibloc='./checkdisk debug_fill'
alias frbloc='./checkdisk debug_free'
alias mkhd='./checkdisk mkhd'
alias dstat='./checkdisk status'
alias dclean='make clean; rm -f vdisk.*'
