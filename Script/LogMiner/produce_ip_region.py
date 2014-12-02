#encoding:utf-8
file = open("ip_region.ini", "r")

file1 = open("asn2.txt", "r")
asn_name = {}
for line in file1:
	words = line.split()
	asn_name[words[0]] = words[1]

user = []
for line in file:
	words = line.replace("\n", "").split(",")
	aaaa = []
	aaaa.append(words[0])
	aaaa.append(words[1])
	aaaa.append(words[2])
	user.append(aaaa)
file.close()
file1.close()

for each in user:
	if asn_name.has_key(each[1]):
		name = asn_name[each[1]]
	else:
		each.append("13")
		continue
	if  name == "telecom":
		each.append("1")
	elif name == "unicom":
		each.append("2")
	elif name == "cernet":
		each.append("3")
	elif name == "chinacache":
		each.append("4")
	elif name == "cstnet":
		each.append("5")
	elif name == "cmcc":
		each.append("6")
	elif name == "tietong":
		each.append("7")
	elif name == "foreign":
		each.append("8")
	elif name == "dianxintong":
		each.append("9")
	elif name == "21Vianet":
		each.append("10")
	elif name == "chinagov":
		each.append("11")
	elif name == "cnnic":
		each.append("12")
	elif name == "other":
		each.append("13")


file = open("ip_region.ini1", "w")
for each in user:
	str = each[0] + "," + each[1] + "," + each[2] + "," + each[3]  + "\n"
	file.write(str)
file.close()





