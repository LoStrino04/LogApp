#include <vector>
#include <iostream>
#include <string>

using namespace std;

class LogMsg {
private:
	double timestamp;
	double data;
	int id;
	int channel;
	string name;
public:
	LogMsg(double in_time = 0.0, double in_data = 0.0, int in_id = -1, int in_channel = -1, string in_name = "") {
		timestamp = in_time;
		data = in_data;
		id = in_id;
		channel = in_channel;
		name = in_name;
	}
	double get_timestamp() const { return timestamp; }
	double get_data() const { return data; }
	int get_id() const { return id; }
	int get_channel() const { return channel; }
};

const LogMsg NULL_LOG;

class LogSignal {
private:
	vector<LogMsg> log_messages;
	string signal_name;
public:
	LogSignal() {}
	LogSignal(string in_name, vector<LogMsg> in_logs) {
		log_messages = in_logs;
		signal_name = in_name;
	}
	vector<LogMsg> get_logs() { return log_messages; }
	string get_name() { return signal_name; }
	void set_logs(vector<LogMsg> in_logs) { log_messages = in_logs; }
	void set_name(string in_name) { signal_name = in_name; }
	LogMsg first() {
		if (!log_messages.empty()) return log_messages[0];
		return NULL_LOG;
	}
	void add_log_msg(LogMsg add_log) { log_messages.push_back(add_log); }
};
