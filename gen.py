import argparse
import xml.etree.ElementTree as ET
import os
from jinja2 import Environment, FileSystemLoader
import math
import sys 
import array as arr 

context = {}
print ('ACTGraph (Courtesy: Jinja 2.0)...')

context['XWARE'] = sys.argv[1]
context['RUNNING_SYNTHESIS'] = sys.argv[2]
context['NUM_PEs'] = int(sys.argv[3])
context['RUN_IN_ASYNC_MODE'] = int(sys.argv[4])

context['FPGA_IMPL'] = 0
context['NUM_VALID_PEs'] = 1      
# context['NUM_VALID_PEs'] = 4        
# context['NUM_VALID_PEs'] = context['NUM_PEs']
context['NUM_VALID_HBM_CHANNELS'] = context['NUM_VALID_PEs'] # context['NUM_PEs'] #6, context['NUM_PEs']*
context['EDGE_PACK_SIZE_POW'] = 4 
context['EDGE_PACK_SIZE'] = 2**context['EDGE_PACK_SIZE_POW']
context['HBM_AXI_PACK_SIZE'] = context['EDGE_PACK_SIZE'] 

print ('Generating sources... ')
print ('XWARE: ' + str(context['XWARE']))
print ('RUNNING_SYNTHESIS: ' + str(context['RUNNING_SYNTHESIS']))
print ('NUM_PEs: ' + str(context['NUM_PEs']))
print ('RUN_IN_ASYNC_MODE: ' + str(context['RUN_IN_ASYNC_MODE']))
print ('NUM_VALID_PEs: ' + str(context['NUM_VALID_PEs']))
print ('NUM_VALID_HBM_CHANNELS: ' + str(context['NUM_VALID_HBM_CHANNELS']))
print ('EDGE_PACK_SIZE: ' + str(context['EDGE_PACK_SIZE']))
print ('HBM_AXI_PACK_SIZE: ' + str(context['HBM_AXI_PACK_SIZE']))

context['II_CREATE_ACTPACK_LOOP1'] = 2
context['II_CREATE_ACTPACK_LOOP2'] = 2
context['II_LOAD_EDGEUPDATES_LOOP1'] = 4 
context['II_COLLECTSTATS_EDGEUPDATES_LOOP1'] = 2 
context['II_COLLECTSTATS_EDGEUPDATES_LOOP2'] = 2
context['II_APPLY_EDGEUPDATES_MAINLOOP1D'] = 2
context['II_SAVEMISSES_EDGEUPDATES_MAINLOOP1'] = 1
context['II_SAVEMISSES_EDGES_MAINLOOP1'] = 1
context['II_APPLY_EDGEUPDATES_RESETURAMBUFFERS_MAINLOOP1'] = 2
context['II_PROCESS_EDGES_MAINLOOP1D'] = 1
context['II_SAVE_VERTEXUPDATES_MAINLOOP1B'] = 1
context['II_READ_DEST_PROPERTIES_LOOP2B'] = 1
context['II_APPLY_UPDATES_LOOP1'] = 3 #1
context['II_SAVE_DEST_PROPERTIES_LOOP2'] = 1
context['II_COLLECT_FRONTIERS_LOOP1B'] = 1
context['II_SAVE_FRONTIERS_LOOP2B'] = 1
context['II_TRANSPORT_FRONTIER_PROPERTIES_LOOP1B'] = 1
context['II_READ_FRONTIERS_1A'] = 1
context['II_READ_FRONTIERS_1B'] = 2 #1

# context['II_CREATE_ACTPACK_LOOP1'] = 16
# context['II_CREATE_ACTPACK_LOOP2'] = 16
# context['II_LOAD_EDGEUPDATES_LOOP1'] = 16 
# context['II_COLLECTSTATS_EDGEUPDATES_LOOP1'] = 16 
# context['II_COLLECTSTATS_EDGEUPDATES_LOOP2'] = 16
# context['II_APPLY_EDGEUPDATES_MAINLOOP1D'] = 16
# context['II_SAVEMISSES_EDGEUPDATES_MAINLOOP1'] = 16
# context['II_SAVEMISSES_EDGES_MAINLOOP1'] = 16
# context['II_APPLY_EDGEUPDATES_RESETURAMBUFFERS_MAINLOOP1'] = 16
# context['II_PROCESS_EDGES_MAINLOOP1D'] = 16
# context['II_SAVE_VERTEXUPDATES_MAINLOOP1B'] = 16
# context['II_READ_DEST_PROPERTIES_LOOP2B'] = 16
# context['II_APPLY_UPDATES_LOOP1'] = 16
# context['II_SAVE_DEST_PROPERTIES_LOOP2'] = 16
# context['II_COLLECT_FRONTIERS_LOOP1B'] = 16
# context['II_SAVE_FRONTIERS_LOOP2B'] = 16
# context['II_TRANSPORT_FRONTIER_PROPERTIES_LOOP1B'] = 16
# context['II_READ_FRONTIERS'] = 16

###

# relref="../"
relref=""

o_path0=relref+"include/common.h"
o_path1=relref+"acts_templates/acts_kernel.cpp"
o_path2=relref+"acts_templates/acts_kernel.h"
o_path3=relref+"acts_templates/acts_kernel.cpp"
o_path4=relref+"acts_templates/acts_kernel.h"
o_path5=relref+"include/common.h"
o_path6=relref+"include/common.h"
o_path7=relref+"include/common.h"
o_path8=relref+"include/common.h"
o_path9=relref+"include/common.h"
o_path10=relref+"include/common.h"

out_path0=os.path.abspath(o_path0)
out_path1=os.path.abspath(o_path1)
out_path2=os.path.abspath(o_path2)
out_path3=os.path.abspath(o_path3)
out_path4=os.path.abspath(o_path4)
out_path5=os.path.abspath(o_path5)
out_path6=os.path.abspath(o_path6)
out_path7=os.path.abspath(o_path7)
out_path8=os.path.abspath(o_path8)
out_path9=os.path.abspath(o_path9)
out_path10=os.path.abspath(o_path10)

templ_path0=relref+"include/"
templ_path1=relref+"acts_templates/"
templ_path2=relref+"acts_templates/"
templ_path3=relref+"acts_templates/"
templ_path4=relref+"acts_templates/"
templ_path5=relref+"include/"
templ_path6=relref+"include/"
templ_path7=relref+"include/"
templ_path8=relref+"include/"
templ_path9=relref+"include/"
templ_path10=relref+"include/"

###
context['1_seq'] = []
for i in range (0,1):
		context['1_seq'].append(i)
        
context['4_seq'] = []
for i in range (0,4):
		context['4_seq'].append(i)
 
# context['HBM_FANOUT'] = 4
context['HBM_FANOUT'] = context['NUM_VALID_HBM_CHANNELS'] # 4 
# context['HBM_FANOUT_CHANNELS'] = 1
# context['HBM_FANOUT_CHANNELS'] = 4
context['HBM_FANOUT_CHANNELS'] = 64
# context['HBM_FANOUT_seq'] = []
# for i in range (0,context['HBM_FANOUT']):
		# context['HBM_FANOUT_seq'].append(i)          

context['8_seq'] = []
for i in range (0,8):
		context['8_seq'].append(i)
        
context['NUM_PEs_seq'] = []
for i in range (0,(context['NUM_PEs'])):
		context['NUM_PEs_seq'].append(i)
        
context['NUM_VALID_PEs_seq'] = []
for i in range (0,(context['NUM_VALID_PEs'])):
		context['NUM_VALID_PEs_seq'].append(i)
        
context['NUM_VALID_HBM_CHANNELS_seq'] = []
for i in range (0,(context['NUM_VALID_HBM_CHANNELS'])):
		context['NUM_VALID_HBM_CHANNELS_seq'].append(i)
        
context['HBM_AXI_PACK_SIZE_seq'] = []
for i in range (0,(context['HBM_AXI_PACK_SIZE'])):
		context['HBM_AXI_PACK_SIZE_seq'].append(i)
 
context['EDGE_PACK_SIZE_seq'] = []
for i in range (0,(context['EDGE_PACK_SIZE'])):
		context['EDGE_PACK_SIZE_seq'].append(i) 
        
env0 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path0)), trim_blocks=True, lstrip_blocks=True)
env1 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path1)), trim_blocks=True, lstrip_blocks=True)
env2 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path2)), trim_blocks=True, lstrip_blocks=True)
env3 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path3)), trim_blocks=True, lstrip_blocks=True)
env4 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path4)), trim_blocks=True, lstrip_blocks=True)
env5 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path5)), trim_blocks=True, lstrip_blocks=True)
env6 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path6)), trim_blocks=True, lstrip_blocks=True)
env7 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path7)), trim_blocks=True, lstrip_blocks=True)
env8 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path8)), trim_blocks=True, lstrip_blocks=True)
env9 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path9)), trim_blocks=True, lstrip_blocks=True)
env10 = Environment(loader=FileSystemLoader(os.path.abspath(templ_path10)), trim_blocks=True, lstrip_blocks=True)

env0.globals.update(zip=zip)
env1.globals.update(zip=zip)
env2.globals.update(zip=zip)
env3.globals.update(zip=zip)
env4.globals.update(zip=zip)
env5.globals.update(zip=zip)
env6.globals.update(zip=zip)
env7.globals.update(zip=zip)
env8.globals.update(zip=zip)
env9.globals.update(zip=zip)
env10.globals.update(zip=zip)

template0 = env0.get_template('common_h.template')
template1 = env1.get_template('acts_kernel.template')
template2 = env2.get_template('acts_kernel_h.template')
template3 = env3.get_template('acts_kernel.template')
template4 = env4.get_template('acts_kernel_h.template')
template5 = env5.get_template('common_h.template')
template6 = env6.get_template('common_h.template')
template7 = env7.get_template('common_h.template')
template8 = env8.get_template('common_h.template')
template9 = env9.get_template('common_h.template')
template10 = env10.get_template('common_h.template')

rendered_file0 = template0.render(context=context)
rendered_file1 = template1.render(context=context)
rendered_file2 = template2.render(context=context)
rendered_file3 = template3.render(context=context)
rendered_file4 = template4.render(context=context)
rendered_file5 = template5.render(context=context)
rendered_file6 = template6.render(context=context)
rendered_file7 = template7.render(context=context)
rendered_file8 = template8.render(context=context)
rendered_file9 = template9.render(context=context)
rendered_file10 = template10.render(context=context)

with open(out_path0, 'w') as outFile0:
	outFile0.write(rendered_file0)
with open(out_path1, 'w') as outFile1:
	outFile1.write(rendered_file1)
with open(out_path2, 'w') as outFile2:
	outFile2.write(rendered_file2)
with open(out_path3, 'w') as outFile3:
	outFile3.write(rendered_file3) 
with open(out_path4, 'w') as outFile4:
	outFile4.write(rendered_file4) 
with open(out_path5, 'w') as outFile5:
	outFile5.write(rendered_file5)
with open(out_path6, 'w') as outFile6:
	outFile6.write(rendered_file6)
with open(out_path7, 'w') as outFile7:
	outFile7.write(rendered_file7)
with open(out_path8, 'w') as outFile8:
	outFile8.write(rendered_file8)
with open(out_path9, 'w') as outFile9:
	outFile9.write(rendered_file9)
with open(out_path10, 'w') as outFile10:
	outFile10.write(rendered_file10)

print ("successful!")
print ("...")
quit()




