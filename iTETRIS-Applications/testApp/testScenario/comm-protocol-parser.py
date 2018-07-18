import os
import argparse
import sys

class ParsedFile(object):
    
    def __init__(self, args, filename):
        self.fileName = filename
        self.args = args
        self.simEnded = False
        
    def parse(self):
        time = 0
        logFile = open(self.fileName, 'r')
        
        self.totals = dict.fromkeys(['timeout', 'last'], 0)
        self.samples = dict.fromkeys(['protocol_quantity', 'protocol_max_distance' , 'protocol_min_distance', 'protocol_total_time', 'protocol_speed', 'protocol_density',
                             'real_density', 'send', 'real_quantity', 'real_speed'])
        for k in self.samples: 
            self.samples[k] = []
        
        for line in logFile:
            vclass = node = None
            tokens = line.split()
            while tokens[0][0] in ['@', '#', '*']:
                tok = tokens.pop(0)
                if tok[0] == '@':
                    time = int(tok[1:])
                    break
                elif tok[0] == '#':
                    node = int(tok[1:])
                elif tok[0] == '*':
                    vclass = tok[1]
                    node = int(tok[2:])    
            
            if len(tokens) == 0:
                continue
            
            if tokens[0] == 'dirs':
                index = 1 if len(tokens) == 2 else 2
                # RSU directions info
                self.directions = [ d for d in tokens[index].split(',')[:-1] ]
            elif tokens[0] == 'n_last':   
                self.totals['last'] += 1             
            elif tokens[0] == 'n_timeout':   
                self.totals['timeout'] += 1
            elif tokens[0] == 'n_send':
                self.samples['send'].append((time, node))
            elif tokens[0] == 'real':                
                tokens.pop(0)
                while len(tokens):
                    tok = tokens.pop(0)
                    subtoks = tok.split('=')
                    if subtoks[0] == 'totc':
                        val = int(subtoks[1])
                        self.samples['real_density'].append((time, val))
                    elif subtoks[0] == ':':
                        # sectors info
                        quantity = dict()
                        speed = dict()
                        while len(tokens):
                            tok = tokens.pop(0)
                            dir = tok.split('=')[0]
                            val = tok.split('=')[1].split(';')
                            quantity[dir] = int(val[0])
                            speed[dir] = float(val[1])
                        self.samples['real_quantity'].append((time, quantity))
                        self.samples['real_speed'].append((time, speed))
            elif tokens[0] == 'flow':
                tokens.pop(0)
                quantity = dict()
                max_distance = dict()
                min_distance = dict()
                speed = dict()
                totTime = dict()
                caltDensity = True
                totc = tokens[0].split('=')                
                if totc[0] == 'totc':
                    tokens.pop(0)
                    self.samples['protocol_density'].append((time, int(totc[1])))
                    caltDensity = False
                while len(tokens):
                    tok = tokens.pop(0).split('=')
                    vals = tok[1].split(':')
                    lenVal = len(vals)
                    quantity[tok[0]] = int(vals[0])
                    val = float(vals[1])
                    if val > 0:  # distance > 0
                        max_distance[tok[0]] = val
                    val = float(vals[2])
                    if val >= 0:  # speed >= 0
                        speed[tok[0]] = val
                    val = float(vals[3])
                    if val >= 0:
                        totTime[tok[0]] = val
                    val = float(vals[4])
                    if val >= 0:
                        min_distance[tok[0]] = val                    
                self.samples['protocol_quantity'].append((time, quantity))
                self.samples['protocol_max_distance'].append((time, max_distance))
                self.samples['protocol_min_distance'].append((time, min_distance))
                self.samples['protocol_total_time'].append((time, totTime))
                self.samples['protocol_speed'].append((time, speed))
                if caltDensity:
                    density = 0
                    for key in quantity:
                        density += quantity[key]
                    self.samples['protocol_density'].append((time, density))
            elif tokens[0] == 'end':
                # post-sim totals
                self.simEnded = True
                self.simTime = time / 1000
                self.totals['time'] = self.simTime
                while len(tokens):
                    toks = tokens.pop().split('=')
                    if len(toks) == 2:
                        self.totals[toks[0]] = float(toks[1])
        if not self.simEnded:
            self.simTime = time / 1000
        logFile.close()

    def newSample(self, key): 
        result = None       
        if key in ['real_density','protocol_density', 'send' ]:
            result = {'val': 0, 'valid': False}
        elif key in [ 'real_quantity' , 'real_speed' , 'protocol_quantity' , 'protocol_max_distance' ,
                    'protocol_min_distance', 'protocol_total_time', 'protocol_speed']:
            result = {'val': dict.fromkeys(self.directions, 0.0), 'valid': dict.fromkeys(self.directions, 0)}
        return result
    
    def addValue(self, key, current, sample):
        if sample == None:
            sample = self.newSample(key)
        if key in ['real_density','protocol_density']:
            sample['val'] = current
            sample['valid'] = True
        elif key in ['send']:
            sample['val'] += 1
            sample['valid'] = True
        elif key in ['real_quantity' , 'real_speed' , 'protocol_quantity' , 'protocol_max_distance' , 'protocol_min_distance',
              'protocol_total_time', 'protocol_speed']:
            for dir in current:
                sample['val'][dir] += current[dir]
                sample['valid'][dir] += 1    
        return sample
        
    def prepareSample(self, key, sample, time):
        if type(sample['val']) == dict:
            for dir in sample['val']:
                if sample['valid'][dir] == 0:
                    sample['valid'][dir] = False
                else:
                    sample['val'][dir] /= sample['valid'][dir]
                    sample['valid'][dir] = True
        self.aggregate[key][time] = sample        

    def aggregateData(self):
        self.aggregate = dict()
        for key in self.samples:            
            self.aggregate[key] = dict()
            t = 0
            next_time = self.args.time
            sample = None
            for (time, data) in self.samples[key]:
                while time >= next_time:
                    if sample != None:
                        self.prepareSample(key, sample, t)
                        sample = None
                    t = int(next_time / self.args.time)
                    next_time += self.args.time                          
                sample = self.addValue(key, data, sample)        
                
def sumValue(key, current, data, dirs):    
    if key in ['real_density','protocol_density', 'send']:
        if data == None:
            data = {'val': 0, 'num': 0}
        if current['valid']:
            data['val'] += current['val']
            data['num'] += 1
    elif key in ['real_quantity' , 'real_speed' , 'protocol_quantity' , 'protocol_max_distance' , 'protocol_min_distance',
                 'protocol_total_time', 'protocol_speed']:
        if data == None:
            data = {'val': dict.fromkeys(dirs, 0.0), 'num': dict.fromkeys(dirs, 0)}
        for dir in dirs:            
            if current['valid'][dir]:
                data['val'][dir] += current['val'][dir]
                data['num'][dir] += 1
    return data
        
def averageValues(key, data, dirs):    
    if key in ['real_density','protocol_density', 'send']:
        if data['num'] > 0:
            return data['val'] / data['num']
        return None
    elif key in ['real_quantity' , 'real_speed' , 'protocol_quantity' , 'protocol_max_distance' , 'protocol_min_distance',
                 'protocol_total_time', 'protocol_speed']:        
        for dir in dirs:
            if data['num'][dir] > 0:
                data['val'][dir] = data['val'][dir] / data['num'][dir]
            else:
                data['val'][dir] = None
        return data['val']
            
def averageSimulations(files):
    result = dict()
    for key in files[0].aggregate:        
        timeTable = dict()
        for file in files:
            for time in file.aggregate[key]:                
                if not time in timeTable:
                    timeTable[time] = None
                timeTable[time] = sumValue(key, file.aggregate[key][time], timeTable[time], file.directions)
        result[key] = dict()
        for time in timeTable:
            result[key][time] = averageValues(key, timeTable[time], files[0].directions)
    return result

def getBaseName(name):    
    return name.split('.')[0]

def parseFiles(args, fileName):
    inputFiles = []
    if not args.multiple_files:
        inputFiles.append(fileName)
        baseName = fileName
    else:
        baseName = getBaseName(fileName)
        print(baseName)
        for f in os.listdir('.'):
            if os.path.isfile(os.path.join('.', f)) and getBaseName(f) == baseName and os.path.splitext(f)[1] == os.path.splitext(fileName)[1]:
                inputFiles.append(f)
    
    numSim = len(inputFiles)
    print("Parsing %d input files with base name %s" % (numSim, baseName))
    
    files = []
    warnTruncated = True
    for f in inputFiles:
        p = ParsedFile(args, f)
        p.parse()
        if not p.simEnded and warnTruncated:
            answer = ''
            while answer not in ['y', 'n', 'a']:
                print("\nSimulation log %s is truncated! Continue anyway?" % f)
                answer = input('[(Y)es, (N)o, yes to (A)ll:').lower()
            if answer == 'n': 
                exit('Aborted.')
            elif answer == 'a': 
                warnTruncated = False
        p.aggregateData()
        files.append(p)
        print('.', end='')
        sys.stdout.flush()
        
    data = averageSimulations(files)
    print(" done\nWriting output...", end='')
    outfile = open(getBaseName(fileName) + '.csv', 'w')
    
    # collect all keys and timings
    keyList = ['time', 'dropped']
    for key in sorted(data):
        if key in ['send', 'real_density','protocol_density']:
            keyList.append(key)
        else:
            for dir in files[0].directions:
                keyList.append(key + '_dir_' + str(dir))

    str_format = '%s;' * len(keyList) + '\n'
    outfile.write(str_format % tuple(keyList))
    for time in range(0, int(files[0].simTime + 1)):
        record = [time, '']
        for key in keyList[2:]:
            if len(key.split('_dir_')) > 1:
                key, dir = key.split('_dir_')
            val = ''
            if time in data[key]:
                val = data[key][time]
                if type(val) == dict:
                    if dir in val:
                        val = val[dir]   
                if val == None:
                    val = ''                 
            record.append(val)
        outfile.write(str_format % tuple(record))

    outfile.close()
    print('done')
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('logFile')
    parser.add_argument('-t', '--time', default=1000, type=int, help='Step time in ms.')
    parser.add_argument('-m', '--multiple-files', action="store_true", help='Parse and average multiple simulation files.')
    args = parser.parse_args()
    
    parseFiles(args, args.logFile)
