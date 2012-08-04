struct adapter {
	unsigned long in_dev;
	unsigned long out_dev;
	unsigned long in_diff;
	unsigned long out_diff;
	unsigned long in_dev_last;
	unsigned long out_dev_last;
	int gotbase;
	unsigned long megcounti, megcounto;
	unsigned long megi;
	unsigned long mego;
	unsigned long dbsync_in_diff;
	unsigned long dbsync_out_diff;
};

void runttraffd(std::string url, int clientid, std::string authKey);
bool write_to_db(std::string url, int clientid, std::string authKey, int day, int month, int year, std::string ifname, unsigned long in_diff, unsigned long out_diff);
unsigned long get_indiff(std::string iface);
unsigned long get_outdiff(std::string iface);