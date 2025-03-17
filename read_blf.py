import can
import cantools

#creazione classe Log per i dati letti dal blf
class Log:
    def __init__(self, in_time = -1 , in_data = -1, in_id = -1, in_channel = -1, in_name = ""):
        self._time = in_time
        self._data = in_data
        self._id = in_id
        self._channel = in_channel
        self._name = in_name
    
    def timestamp(self) -> float:
        return self._time
    def data(self) -> float:
        return self._data
    def id(self) -> int:
        return self._id
    def channel(self) -> int:
        return self._channel
    def name(self) -> str:
        return self._name
    def set_name(self, in_name):
        self._name = in_name

def main():
    # Percorso files dbc
    dbc_paths = []
    # Percorso del file BLF
    blf_file = ""
    # File .txt dove scrivere i dati del log
    txt_file = "TXT\\log_datas.txt"
    # Liste per memorizzare i dati
    logs = []
    messages = []

    with open("TXT\\blf_name.txt") as blf_name:
        blf_file = blf_name.readline()

    with open("TXT\\dbc_names.txt") as dbc_file:
        for dbc_path in dbc_file:
            dbc_paths.append(dbc_path[:len(dbc_path) - 1])
    
    # Aprire e leggere il file BLF e creazione di una lista di tuple (id,data,nome) con un elemento per ogni id
    with can.BLFReader(blf_file) as log:
        for msg in log:
            tmp_log = Log(msg.timestamp,int.from_bytes(msg.data, byteorder='little'),msg.arbitration_id,msg.channel, "")
            logs.append(tmp_log)

            id_found = False     
            for cont_msg in messages:
                if (cont_msg[0] == msg.arbitration_id):
                    id_found = True
            if id_found:
                continue
            
            messages.append((msg.arbitration_id, msg.data, ""))

    # cerco in ogni dbc il nome corrispondente ad ogni id nella lista precedentemente creata
    with open("TXT\\id_names.txt", "a") as id_names_file:
        for db_path in dbc_paths:
            db = cantools.database.load_file(db_path)
            for msg_tuple in messages:
                if msg_tuple[2] != "":
                    continue
                
                tmp_name = ""
                try:
                    message = db.get_message_by_frame_id(msg_tuple[0])
                    decoded_signals = message.decode(msg_tuple[1])
                    # Iterate over signal names and values
                    for signal_name, signal_value in decoded_signals.items():
                        tmp_name = signal_name                   
                except:
                    continue

                msg_tuple = msg_tuple[0],msg_tuple[1],tmp_name
                id_names_file.write(f"{msg_tuple[0]},{tmp_name}")
                id_names_file.write("\n")

        id_names_file.close()
    

    # Scrivo sul txt i dati di ogni messaggio del log in formato csv 
    log_txt = open(txt_file,"a")
    for index in range(0,len(logs)):
        log_txt.write(f"{logs[index].timestamp()},{logs[index].data()},{logs[index].id()},{logs[index].channel()},{logs[index].name()}")
        log_txt.write("\n")
    log_txt.close()
    

if __name__ == "__main__":
    main()
