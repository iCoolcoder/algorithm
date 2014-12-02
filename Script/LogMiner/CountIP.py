#!/usr/bin/python
#encoding:utf-8
import re,time
import codecs
import region_map

class stat:
	def __init__(self, output_file):
		self.output = codecs.open(output_file, "wb", "utf-8")
		self.region_dict = region_map.region_dict
		self.sorted_region_dict_keys = sorted(self.region_dict.keys())
		self.province_dict = region_map.province_dict
		self.ip_count_dict = {}
		pass
		
	def __del__(self):
		self.output.close()
		
	def load_ip_region(self, file_path):
		file = open(file_path, 'r')
		self.ip_region = {}
		for line in file:
			rec = []
			words = line.split(',')
			start_ip, end_ip = get_start_and_end_ip(words[0])
			asn = int(words[1])
			region = int(words[2])
			rec.append(start_ip)
			rec.append(end_ip)
			rec.append(asn)
			rec.append(region)
			self.ip_region[start_ip] = rec
			
	'''
	purpose:
	get ip and corresponding times it appear
	'''
	def mail_log(self, file_path):
		log=open(file_path, 'r')
		C=r'\.'.join([r'\d{1,3}']*4)
		find=re.compile(C)
		count={}
		for i in log:
			for ip in find.findall(i):
				count[ip]=count.get(ip, 1) + 1
		return count

	def time2stamp(self, timestr, format_type='[%d/%m/%Y:%H:%M:%S+0800]'):
		temp = ''
		if (timestr.find('Sep') != -1):
			temp = timestr.replace('Sep', '9')
		elif (timestr.find('Oct') != -1):
			temp = timestr.replace('Oct', '10')
		return time.mktime(time.strptime(temp, format_type))		

	'''
	purpose:
	sort the dictionary and return the keys within a list
	'''		
	def sort_by_value(self, d): 
		items=d.items() 
		backitems=[[v[1],v[0]] for v in items] 
		backitems.sort() 
		return [ backitems[i][1] for i in range(0,len(backitems))]		
				
	'''
	purpose: 
	get flow rate of statistics
	parameters: 
	@file_path: file location
	@pastime:  the time span before now
	@interval: time interval we get one rate point 
	return:
	frame rates.
	usage:
	rates = analyze_flow_rate('mylog2', 36000, 60)
	for k, v in rates.items():
			print v
	'''
	def analyze_flow_rate(self, file_path, pastime, interval):
		now = time.time()
		start_time = now - pastime
		rates = {}
		num = 0
		stuck_users = {}
		total_users = {}
		log = open(file_path, 'r')
		for line in log:
			words = line.split()
			line_ip = words[0]
			line_time = self.time2stamp(words[3] + words[4])
			line_url = words[6]
			end_time = start_time + interval

			if start_time <= line_time < end_time:
				if line_url.find('?t=pd') != -1:
					stuck_users[line_ip] = 1
				elif line_url.find('?t=cv1') != -1:
					total_users[line_ip] = 1
			elif line_time < start_time:
				continue
			else:
				if len(total_users) > 0:
					rates[num] = 1 - float(len(stuck_users)) / float(len(total_users))
					num += 1
				stuck_users.clear()
				total_users.clear()
				start_time += interval
				if start_time <= line_time < end_time:
					if line_url.find('?t=pd') != -1:
						stuck_users[line_ip] = 1
					elif line_url.find('?t=cv1') != -1:
						total_users[line_ip] = 1
		return rates

	def print_flow_rate_per_user(self, file_path):
		rates = self.analyze_flow_rate_per_user(file_path)
		items = self.sort_by_value(rates)
		for k in items:
			print k + ':   ' + str(rates[k])
		
	'''
	purpose:
	get every user's flow rate in one day
	usage:
	rates = analyze_flow_rate_per_user('mylog')
	items = sort_by_value(rates)
	for k in items:
		print k + ':   ' + str(rates[k])
	'''	
	def analyze_flow_rate_per_user(self, file_path, from_time = 0, to_time = 22222222222222):
		stuck_time_per_user = {}
		total_time_per_user = {}
		log = open(file_path, "r")
		for line in log:
			words = line.split()
			line_ip = words[0]
			line_url = words[6]
			line_time = self.time2stamp(words[3] + words[4])
			if from_time <= line_time < to_time:
				if line_url.find('?t=pd') != -1:
					stuck_time_per_user[line_ip] = stuck_time_per_user.get(line_ip, 1) + 1
				elif line_url.find('?t=cv1') != -1:
					total_time_per_user[line_ip] = total_time_per_user.get(line_ip, 1) + 1
		rates = {}
		times = {}
		for k, v in stuck_time_per_user.items():
			if k in total_time_per_user.keys():
				times[k] = float(total_time_per_user[k])
				rates[k] = float(stuck_time_per_user[k]) / float(total_time_per_user[k])
		return times, rates
		
	def get_start_and_end_ip(self, s):
		parts = s.split('/')
		sub_parts = parts[0].split('.')
		start_ip = 0
		end_ip = 0
		start_ip = long(sub_parts[0])
		for i in range(1, 4):
			start_ip = (start_ip << 8) | long(sub_parts[i])
		mask = long(parts[1])
		end_ip = start_ip | ((2 ** (32 - mask))  -1)
		return start_ip, end_ip
		
	'''
	usage:
	asn, region = find_ip_region(ip)
	'''
	def find_ip_region(self, str_ip):
		sub_parts = str_ip.split('.')
		ip = long(sub_parts[0])
		for i in range(1, 4):
			ip = (ip << 8) | long(sub_parts[i])
		low = 0
		high = len(self.sorted_region_dict_keys)
		while low < high:
			if high - low <= 20:
				last_item = []
				for idx in range(low, high - 1):
					t = self.region_dict[self.sorted_region_dict_keys[idx]]
					if t[0] <= ip and ip <= t[1]:
						if len(last_item) != 0:
							if t[1] - t[0] < last_item[1] - last_item[0]:
								last_item = t
						else:
							last_item = t
				if len(last_item) != 0:
					return last_item[2], last_item[3]
					
			mid = (low + high) / 2
			t = self.region_dict[self.sorted_region_dict_keys[mid]]
			if t[0] > ip:
				high = mid
			elif t[1] < ip:
				low = mid + 1
			else:
				return t[2], t[3]
		return -1, -1
	

	def load_user_dictribution_log(self, file_path):
		log = open(file_path, "r")
		for line in log:
			words = line.split()
			line_ip = words[0]
			self.ip_count_dict[line_ip] = self.ip_count_dict.get(line_ip, 1) + 1
		log.close()
	
	'''
	usage: count users distribution per ip
	'''
	def analyze_user_distribution(self, file_path):
		self.load_user_dictribution_log(file_path)
		region_users = {}
		ip_count_dict_keys = self.ip_count_dict.keys()
		for k in ip_count_dict_keys:
			a, b = self.find_ip_region(k)
			if b != -1 and self.region_dict.has_key(b):
				region_users[b] = region_users.get(b, 1) + 1
		
		region_users_items = self.sort_by_value(region_users)
		provice_users = {}
		for k in self.province_dict.keys():
			provice_users[k] = provice_users.get(k, 0)
		
		for k in region_users_items:
			if provice_users.has_key(str(k)[0:2]):
				provice_users[str(k)[0:2]] = provice_users[str(k)[0:2]] + region_users[k]
			
		for k, v in provice_users.items():
			str_line = self.region_dict[int(str(k)+"0000")].decode("utf-8") + " : " + str(v)
			print str_line
			self.output.write(str_line + '\n')
	
	def flush_output_file(self):
		self.output.flush()
		
if __name__ == '__main__':
	print time.clock()
	a = stat("data4")
	a.analyze_user_distribution("access.log.1")
	a.flush_output_file()
	print 'time past(%s)'%(time.clock())