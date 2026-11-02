import os
import sys

"""generate	source file	list for Android.mk"""
#TODO1 limit mk	file num in	one	mk file
#TODO2 traverse	level

#MK_GEN_PATH = "jni/gen"
MK_GEN_PATH=""
#SRC_ROOT =	"../../NewDrawLib"
SRC_ROOT = ""
SRC_EXT	= [".c", ".cpp", ".cc"]
#EXCLUDE_PATTERN = [	"FontCreatorGDI", "NPRenderWindowEGL",
#					"FontCreatorIOS", "Win32Ime"]
EXCLUDE_PATTERN	= []

MK_FILE	= 0
MK_FILENAME	= ""
MK_FILECOUNT = 0
JNI_ABSPATH	= ""

TOTAL_FILE_COUNT = 0

def	start_mk(relpath_src):
	global MK_FILE
	global MK_FILENAME
	global JNI_ABSPATH
	#print "why"
	subdir_split = relpath_src.split(os.sep)
	#print(subdir_split)
	MK_FILENAME	= "_".join(subdir_split)
	#print(MK_FILENAME)
	MK_FILENAME	+= ".mk"
	print "MK_FILENAME = " + MK_FILENAME
	MK_FILENAME	= "Android.mk"
	
	if MK_FILE == 0:
		MK_FILE	= open(os.path.join(MK_GEN_PATH, MK_FILENAME), 'w')
	
	pass

def	end_mk():
	global MK_FILE
	global MK_FILENAME
	global JNI_ABSPATH
	global MK_FILECOUNT
	if MK_FILE != 0:
		size = MK_FILE.tell()
		#MK_FILE.close()
		#if (size ==	0):
		#	os.remove(os.path.join(MK_GEN_PATH,MK_FILENAME))
		
	#MK_FILE	= 0
	MK_FILENAME	= ""
	MK_FILECOUNT = 0
	pass

def	switch_mk(relpath_src):
	end_mk()
	start_mk(relpath_src)
	pass

def	scan_folder(dirpath, filenames):
	global MK_FILE
	global MK_FILENAME
	global JNI_ABSPATH
	global MK_FILECOUNT
	global TOTAL_FILE_COUNT
	#print "  dirpath =	" +	dirpath
	for	filename in	filenames:
		#print "	filename = " + filename
		if os.path.splitext(filename)[1].lower() in	SRC_EXT:
			filename_rel2jni = os.path.relpath(os.path.join(dirpath, filename),	JNI_ABSPATH)
			filename_rel2jni = filename_rel2jni.replace("\\", "/")
			bExclude = False
			for	pattern	in EXCLUDE_PATTERN:
				if filename_rel2jni.find(pattern) >=0 :
					bExclude = True
			if not bExclude:
				MK_FILE.write("LOCAL_SRC_FILES += "	+ filename_rel2jni + "\n")
				MK_FILECOUNT +=	1
				TOTAL_FILE_COUNT +=	1
	pass

def	load_config():
	fconf =	open("Gensrc_conf.txt")
	lines =	fconf.readlines()
	for	line in	lines:
		line = line.strip("\n")
		if not line[0] == '#' :
			EXCLUDE_PATTERN.append(line)

def	check_param():
	global MK_GEN_PATH
	global SRC_ROOT
	if len(sys.argv) <=	2:
		print "useage: python Gensrc.py	output_dir src_dir "
		print "generate	src	file list which	can	be included	in Android.mk"
		print "src_dir : dir storing src files"
		print "output_dir :	dir	to store src list. usually jni/gen"
		print "require python 2.6"
		#sys.exit()
		MK_GEN_PATH	= "jni/gen"
		SRC_ROOT = "../../src"
	else:
		MK_GEN_PATH	= sys.argv[1]
		SRC_ROOT = sys.argv[2]
	print "MK_GEN_PATH=" + MK_GEN_PATH
	print "SRC_ROOT=" +	SRC_ROOT


def run_src_path():
	global SRC_ROOT
	start_mk(os.path.basename(SRC_ROOT))
	for	dirpath, dirnames, filenames in	os.walk(SRC_ROOT):
		try:
			dirnames.remove(".svn")
		except ValueError, e:
			pass

		scan_folder(dirpath, filenames)
	end_mk()
	
	print "%d source files found" %	TOTAL_FILE_COUNT


AndroidMKFilePath = "."+os.sep+"jni"+os.sep+"Android.mk"
AndroidMKFile = 0
MarkLine = "#mark1"
lines = 0
def build_Android_MK_first_part():
	global AndroidMKFilePath
	global AndroidMKFile
	global MK_FILE
	AndroidMKFile = open(AndroidMKFilePath,'r')
	global SRC_ROOT
	start_mk(os.path.basename(SRC_ROOT))

	global lines
	lines =	AndroidMKFile.readlines()
	for	line in	lines:
		#line = line.strip("\n")
		if line.find("LOCAL_SRC_FILES") == -1 :
			MK_FILE.write(line)
		else:
			break
	##


def build_Android_MK_last_part():
	global lines
	global MK_FILE

	bFlag = False

	for	line in	lines:
		print(line)
		if line.find(MarkLine) != -1:
			bFlag = True
		if bFlag == True:
			MK_FILE.write(line)
		#
	#
	print(MarkLine)
#


if __name__	== "__main__":
	check_param()
	JNI_ABSPATH	= os.path.abspath("jni")
	load_config()
	if not os.access(MK_GEN_PATH, os.F_OK):
		os.makedirs(MK_GEN_PATH)

	
	build_Android_MK_first_part()

	run_src_path()
	
	SRC_ROOT = sys.argv[3]
	run_src_path()

	SRC_ROOT = sys.argv[4]
	run_src_path()

	SRC_ROOT = sys.argv[5]
	run_src_path()

	build_Android_MK_last_part()

	if MK_FILE != 0:
		size = MK_FILE.tell()
		MK_FILE.close()
		if (size ==	0):
			os.remove(os.path.join(MK_GEN_PATH,MK_FILENAME))
	#

	AndroidMKFile.close()

	pass
