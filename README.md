Indice:
  - Setup
  - Caricamento BLF
  - Caricamento/ Cancellazione DBC
  - Visualizzazione Grafici
  - Esportazione
  - Consigli / Possibili Errori

Setup: L'applicazione usa uno script python per estrarre i dati dal blf, quindi per eseguirla correttamente serve installare python e le seguenti librerie
       - can ( pip install python-can )
       - cantools (pip install cantools )
       In caso il caricameto del BLF vi risulti completato ma non vedete nessun segnale nella lista degli ID cliccando su uno dei due canali, inserite nel file open_info.txt
       il comando che usa la vostra versione di python per eseguire i file (per esempio py o python)

Caricamento BLF: Copiare il percorso assoluto del file blf nella casella di testo corrispondente, scrivendolo con nei seguenti formato
                 C:\\Users\\Nome\\BLF\\nome.blf
                 C:\Users\Nome\BLF\nome.blf
                 C:/Users/Nome/BLF/nome.blf
                 Può anche essere copiata solo la parte BLF\\nome.blf, solo se il file viene copiato nella cartella BLF contenuta nella cartella dell'applicazione
                 Premere Load Log e aspettare e compaia il popup che conferma il completamento del caricamento
                 
Caricamento DBC: Stessi metodi di caricamento, con percorso assoluto e percorso relativo, una volta inserite nel listbox vi rimarranno anche al riavvio dell'applicazione fino
                 alla loro cancellazione, i segnali i cui nomi non vengon trovati nelle dbc inserite verranno rappresentati dal loro ID

Visualizzazione Grafici: Di default all'avvio se non si seleziona nessuno dei due pulsanti per setttare il grafico da impostare, verrà impostato il grafico superiore, i pulsanti con le freccie
                         servono per spostarsi nel grafico( vengono visualizzati 300 dati per porzione di grafico), alcuni segnali possono creare errori col grafico e far crashare     
                         l'applicazione, se dovesse capitare bisogna riavviare e ricaricare il blf
                         
Esportazione: Possono essere esportati i messaggi di log dei singoli segnali, basta impostare il segnale desiderato su unoo dei due grafic e andare su File->Export CSV->Upper/Lower Plot a     
              seconda del segnale da esportare.
              Per quanto riguarda l'esportazione in png dei grafici è possibile esportare la porzione di grafico visualizzata in quel momento sul grafico, 
              tramite Plot->Export PNG->Upper/Lower Plot

Consigli: Quando si inseriscono i percorsi delle DBC non devono esserci spazi nei nomi di file e cartelle, questo creerebbe un errore nella ssalvataggio del percorso nel file txt 
          corrispondente
          Durante il caricamento del BLF è meglio non selezionare niente nell'interfaccia, questo non porta a un crash dell'applicazione ma potrebbe mandarla in (Non Risponde) fino al 
          completamento del caricamento

