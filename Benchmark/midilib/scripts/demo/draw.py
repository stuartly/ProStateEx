import webbrowser
import sys

state = []
link = dict()
line_split = list()

with open(sys.argv[1]) as f:
    for line in f:
        line_split = line.split("\t")
        if len(line_split) >= 2:
            if line_split[0].count("_") == 0:
                state.append(line_split[1].strip('\n'))
            else:
                link[line_split[0]] = line_split[1]

fo = open("midi.html", "r")
midi_html_str = ''
for line in fo.readlines():
    if line.count('{ "id": 0, "loc":'):
        break
    else:
        midi_html_str += line
fo.close()

f = open("midi.html", "w")
state_idx = 0
json_str = ''
for state_x in state:
    # json_str += '{ "id": ' + str(state_idx) + ', "loc": "0 60", "text": "'+ state_x +'" },\n'
    json_str += '{ "id": ' + str(state_idx) + ', "loc": "'+ str(500*state_idx) +' 60", "text": "'+ state_x +'" },\n'
    # json_str += '{ "id": ' + str(state_idx) + ', "loc": "'+ str(200*(state_idx/4)) + ' ' + str(100*(state_idx/2))+'", "text": "'+ state_x +'" },\n'
    state_idx += 1
json_str = json_str[:-2]
json_str += '],"linkDataArray": ['

from_to = list()
for link_x in link:
    from_state, to_state = link_x.split('_')
    curviness = 80
    if from_state == to_state:
        curviness = -50
    json_str += ' { "from": '+ from_state + ', "to": ' + to_state + ', "text": "'+ link[link_x].strip('\n') + '", "curviness": '+str(curviness)+' },\n'
json_str = json_str[:-2]
json_str += ']}</textarea></div></body></html>'

f.write(midi_html_str + json_str)
f.close()

url = "file:///home/songlh/workspace/protocol/ProtocolEx/stubs/midilib/scripts/midi.html"
webbrowser.open(url, new=0, autoraise=True)


