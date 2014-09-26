#!/usr/bin/python
#encoding:utf-8
import re,time
import codecs
import region_map

'''
purpose:
get ip and corresponding times it appear
'''
def mail_log(file_path):
    log=open(file_path, 'r')
    C=r'\.'.join([r'\d{1,3}']*4)
    find=re.compile(C)
    count={}
    for i in log:
        for ip in find.findall(i):
            count[ip]=count.get(ip, 1) + 1
	return count

def time2stamp(timestr, format_type='[%d/%m/%Y:%H:%M:%S+0800]'):
	temp = ''
	if (timestr.find('Sep') != -1):
		temp = timestr.replace('Sep', '9')
	return time.mktime(time.strptime(temp, format_type))			
			
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
def analyze_flow_rate(file_path, pastime, interval):
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
		line_time = time2stamp(words[3] + words[4])
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

'''
purpose:
sort the dictionary and return the keys within a list
'''		
def sort_by_value(d): 
	items=d.items() 
	backitems=[[v[1],v[0]] for v in items] 
	backitems.sort() 
	return [ backitems[i][1] for i in range(0,len(backitems))]

'''
purpose:
get every user's flow rate in one day
usage:
rates = analyze_flow_rate_per_user('mylog')
items = sort_by_value(rates)
for k in items:
	print k + ':   ' + str(rates[k])
'''	
def analyze_flow_rate_per_user(file_path):
	stuck_time_per_user = {}
	total_time_per_user = {}
	log = open(file_path, 'r')
	for line in log:
		words = line.split()
		line_ip = words[0]
		line_url = words[6]
		if line_url.find('?t=pd') != -1:
			stuck_time_per_user[line_ip] = stuck_time_per_user.get(line_ip, 1) + 1
		elif line_url.find('?t=cv1') != -1:
			total_time_per_user[line_ip] = total_time_per_user.get(line_ip, 1) + 1
	
	rates = {}
	for k, v in stuck_time_per_user.items():
		if k in total_time_per_user.keys():
			rates[k] = float(v) / float(total_time_per_user[k])
	return rates
	
def get_start_and_end_ip(s):
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

	
def load_ip_region(file_path):
	file = open(file_path, 'r')
	ip_region = {}
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
		ip_region[start_ip] = rec
	return ip_region

'''
usage:
sorted_keys = sorted(ip_region.keys())
asn, region = find_ip_region(ip_region, sorted_keys, ip)
'''
def find_ip_region(ip_region, sorted_keys, str_ip):
	sub_parts = str_ip.split('.')
	ip = long(sub_parts[0])
	for i in range(1, 4):
		ip = (ip << 8) | long(sub_parts[i])
	low = 0
	high = len(sorted_keys)
	while low < high:
		if high - low <= 20:
			last_item = []
			for idx in range(low, high - 1):
				t = ip_region[sorted_keys[idx]]
				if t[0] <= ip and ip <= t[1]:
					if len(last_item) != 0:
						if t[1] - t[0] < last_item[1] - last_item[0]:
							last_item = t
					else:
						last_item = t
			if len(last_item) != 0:
				return last_item[2], last_item[3]
				
		mid = (low + high) / 2
		t = ip_region[sorted_keys[mid]]
		if t[0] > ip:
			high = mid
		elif t[1] < ip:
			low = mid + 1
		else:
			return t[2], t[3]
	return -1, -1

def load_region_map(file_path):
	region_map = {}
	file = open(file_path, 'r')
	for line in file:
		sub_parts = line.split(':')
		region_map[long(sub_parts[0])] = sub_parts[1]
	return region_map

if __name__ == '__main__':
	print time.clock()
	output = codecs.open('data1', 'wb', 'utf-8')
	ip_region = load_ip_region('ip_region.ini')
	region_map = region_map.region_dict
	rates = analyze_flow_rate_per_user('mylog1')
	items = sort_by_value(rates)
	for k in items:
		a, b = find_ip_region(ip_region, sorted(ip_region.keys()), k)
		if b != -1 and region_map.has_key(b):
			str_line = k + ':   ' + str(rates[k]) + '      region:   ' + region_map[b].decode('utf-8')
			output.write(str_line)
			print str_line
	output.close()
	print 'time past(%s)'%(time.clock())