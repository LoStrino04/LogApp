import can

#creazione classe Log per i dati letti dal blf
class Log:
    def __init__(self, in_time, in_data, in_id, in_channel):
        self._time = in_time
        self._data = in_data
        self._id = in_id
        self._channel = in_channel
    
    def timestamp(self) -> float:
        return self._time
    def data(self) -> float:
        return self._data
    def id(self) -> int:
        return self._id
    def channel(self) -> int:
        return self._channel

def main():
    # Percorso del file BLF
    blf_file = "LogTorque_013125_160826.blf"
    # File .txt dove scrivere i dati del log
    txt_file = "log_datas.txt"
    # Liste per memorizzare i dati
    logs = []

    # Aprire e leggere il file BLF
    with can.BLFReader(blf_file) as log:
        for msg in log:
            tmp_log = Log(msg.timestamp,int.from_bytes(msg.data, byteorder='little'),msg.arbitration_id,msg.channel)
            logs.append(tmp_log)
            #print(msg)
    
    # Scrivo sul txt i dati di ogni messaggio del log in formato csv 
    log_txt = open(txt_file,"a")
    for index in range(0,len(logs)):
        log_txt.write(f"{logs[index].timestamp()} ,{logs[index].data()}, {logs[index].id()}, {logs[index].channel()}")
        log_txt.write("\n")
    log_txt.close()
    

if __name__ == "__main__":
    main()

    
    
