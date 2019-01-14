import sys
import pickle

if __name__=="__main__":
	sInput = sys.argv[1]

	numIndex = 0
	stateMap = {}
	srcMap = {}
	conditionList = []
	entryList = []

	with open(sInput) as f:
		lines = f.readlines()

		for index, line in enumerate(lines):
			line = line.strip()
			if len(line) == 0:
				continue

			if line.startswith('State = '):
				state = line.replace('=', '==')
				stateMap[state] = numIndex
				numIndex += 1
				continue

			if line.startswith('//---'):
				condition = lines[index-1].strip()
				srcMap[condition] = line
				if not lines[index-1].startswith('State = '):
					conditionList.append(condition) 
				continue

			if line.startswith('=================='):
				entryList.append((conditionList, state))
				conditionList = []
				continue

	for entry in entryList:
		print stateMap[entry[1]],
		print '\t',
		print entry[1]

	print '--------------------------------------------------------------------------'

	for entry in entryList:
		flag = False
		det = -1
		for condition in entry[0]:
			if condition in stateMap:
				flag = True
				det = stateMap[condition]
				break

		if flag:
			print  str(det) + '_' +  str(stateMap[entry[1]]),
			print '\t',
			print '\\n'.join(entry[0])
		else:
			for key in stateMap:
				print str(stateMap[key]) + '_' + str(stateMap[entry[1]]),
				print '\t',
				print '\\n'.join(entry[0])



		
	#pickle.dump(srcMap, open('src.obj', 'wb' ) )

	
