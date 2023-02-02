# Author
**RACHEDI ILYAS**

# Lacement de programmes
***pour lancer les programe on procéde de la facon suivante:***
- la compilation de toute les programe a la fois se fais avec la commande ```make```
- le netoyage de tout les fichier *.o et les executable se fais avec la commande ```make clean```
#### pour l'envoi et reception de packet UDP:
- **compilation**: ```make server```, ```make client```
- **execution** : 
- on comment par activer le server avec ```./server```
- puis on lance le client qui envoie un message automatiquement au sever ```./client```

#### pour lancer une requete DNS
- **compilation** : ```make MyDns```
- **execution** : ```./MyDns hostname```
ps : **hostename** est l'adresse du domaine recherché par exemple "www.google.fr"


### sources
[Dns explication](https://routley.io/posts/hand-writing-dns-messages/)
