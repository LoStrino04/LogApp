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
    # Percorso file dbc
    #dbc_paths = []
    # Percorso del file BLF
    blf_file = "LogTorque_013125_160826.blf"
    # File .txt dove scrivere i dati del log
    txt_file = "log_datas.txt"
    # Liste per memorizzare i dati
    logs = []

    
    #with open("dbc_names.txt") as dbc_file:
        #for dbc_path in dbc_file:
            #dbc_paths.append(dbc_path)

    dbc_paths = ["C:\\Users\\Utente\\Desktop\\Racing Team\\DBC\\VCU_TELEMETRY_CAN.dbc",
                 "C:\\Users\\Utente\\Desktop\\Racing Team\\DBC\\BMS_DEBUG_CAN_old.dbc"]
    db = cantools.database.load_file(dbc_paths[0])
    # Aprire e leggere il file BLF
    with can.BLFReader(blf_file) as log:
        for msg in log:
            tmp_name = ""        
            try:
                msg_id = msg.arbitration_id  # ID del messaggio
                # Cercare il messaggio nel DBC
                message = db.get_message_by_frame_id(msg_id)

                decoded_signals = message.decode(msg.data)
                # Iterate over signal names and values
                for signal_name, signal_value in decoded_signals.items():
                    tmp_name = signal_name                   
            except:
                pass

            print(tmp_name)
            tmp_log = Log(msg.timestamp,int.from_bytes(msg.data, byteorder='little'),msg.arbitration_id,msg.channel, tmp_name)
            logs.append(tmp_log)
                    
    
    # Scrivo sul txt i dati di ogni messaggio del log in formato csv 
    log_txt = open(txt_file,"a")
    for index in range(0,len(logs)):
        log_txt.write(f"{logs[index].name()},{logs[index].timestamp()},{logs[index].data()},{logs[index].id()},{logs[index].channel()}")
        log_txt.write("\n")
    log_txt.close()
    

if __name__ == "__main__":
    main()

    
    
