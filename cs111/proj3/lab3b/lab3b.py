#!/usr/bin/python

# NAME: Andrew Ding, Caleb Chau
# EMAIL: andrewxding@ucla.edu, caleb.h.chau@gmail.com
# ID: 504748356, 204805602

import os
import sys
import csv


fblocks = []
finodes = []
inodes = []
indirects = []
dirents = []
unallocatedInodes = []  # inode numbers only
allocatedInodes = []       # inode objects

nblocks = 0
ninodes = 0
firstinode = 0
errs = 0

class Inode:
	def __init__(self, line):
		self.inodeno = int(line[1])
		self.file_type = line[2]
		self.link_count = int(line[6])
		self.blocks = map(int, line[12:24])
		self.single_indirect = int(line[24])
		self.double_indirect = int(line[25])
		self.triple_indirect = int(line[26])

class Indirect:
	def __init__(self, line):
		self.inodeno = int(line[1])
		self.level = int(line[2])
		self.offset = int(line[3])
		self.reference_no = int(line[5])

class Dirent:
	def __init__(self, line):
		self.parent = int(line[1])
		self.inode_num = int(line[3])
		self.file_name = line[6]

def printExit(msg):
	sys.stderr.write(msg)
	exit(1)


def parseCSV(f):
	global fblocks, finodes, inodes, indirects, nblocks, ninodes, firstinode

	reader = csv.reader(f)
	for line in reader:
		if len(line) <= 0:
			printExit("Error: blank line in csv\n")

		category = line[0]
		if category == 'SUPERBLOCK':
			nblocks = int(line[1])
			ninodes = int(line[2])
			firstinode = int(line[7])
		elif category == 'BFREE':
			fblocks.append(int(line[1]))
		elif category == 'IFREE':
			finodes.append(int(line[1]))
		elif category == 'INODE':
			inodes.append(Inode(line))
		elif category == 'DIRENT':
			dirents.append(Dirent(line))
		elif category == 'INDIRECT':
			indirects.append(Indirect(line))
		elif category != "GROUP":
			printExit("ERROR: invalid type in csv\n")


def audit_blocks():
	global errs

	blocks = {}
	def lvlstr(level):
		if level == 0: 
			l = ""
		if level == 1: 
			l = "INDIRECT "
		if level == 2:
			l = "DOUBLE INDIRECT "
		if level == 3:
			l = "TRIPLE INDIRECT " 
		return l
	def isValidBlock(block, inodeno, offset, level):
		global errs
		if block != 0:
			# Invalid block
			if block >= nblocks:
				print("INVALID {}BLOCK {} IN INODE {} AT OFFSET {}".format(lvlstr(level), block, inodeno, offset))
				errs += 1
			# Reserved block
			elif block < 8:
				print("RESERVED {}BLOCK {} IN INODE {} AT OFFSET {}".format(lvlstr(level), block, inodeno, offset))
				errs += 1

			# Valid block, add inode to block reference count
			elif block in blocks:
				blocks[block].append((level, offset, inodeno))
			else:
				blocks[block] = [(level, offset, inodeno)]

	for inode in inodes:
		# Examine indirect block pointers
		isValidBlock(inode.single_indirect, inode.inodeno, 12, 1)
		isValidBlock(inode.double_indirect, inode.inodeno, 268, 2)
		isValidBlock(inode.triple_indirect, inode.inodeno, 65804, 3)
		# Examine direct block pointer
		for offset, block in enumerate(inode.blocks):
			isValidBlock(block, inode.inodeno, offset, 0)

	# Examine indirect entries
	for indirectBlock in indirects:
		isValidBlock(indirectBlock.reference_no, indirectBlock.inodeno, indirectBlock.offset, indirectBlock.level)

	# Scan non reserved data blocks
	for block in range(8, nblocks):
		# Check if every legal data block is on free block list or allocated to only 1 file
		if block not in fblocks and block not in blocks:
			print("UNREFERENCED BLOCK {}".format(block))
			errs += 1
		# Check if block allocation to some file is on free list
		elif block in fblocks and block in blocks:
			print("ALLOCATED BLOCK {} ON FREELIST".format(block))
			errs += 1
		# Check if block is referenced by multiple files
		elif block in blocks and len(blocks[block]) > 1:
			inode_refs = blocks[block]
			for level, offset, inodeno in inode_refs:
				print("DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(lvlstr(level), block, inodeno, offset))
				errs += 1


def audit_inodes():
	global errs, inodes, allocatedInodes, unallocatedInodes
	# List will contain the correct free inodes
	unallocatedInodes = finodes

	# Scan inodes
	for inode in inodes:
		# unallocated nodes should be on free list
		if inode.file_type == '0': 
			if inode.inodeno not in finodes:
				print("UNALLOCATED INODE {} NOT ON FREELIST".format(inode.inodeno))
				errs += 1
				unallocatedInodes.append(inode.inodeno)
		else:
			if inode.inodeno in finodes: 
				print("ALLOCATED INODE {} ON FREELIST".format(inode.inodeno))
				errs += 1
				unallocatedInodes.remove(inode.inodeno)

			allocatedInodes.append(inode)

	# Scan non-reserved inodes
	for inode in range(firstinode, ninodes):
		used = False
		for i in inodes:
			if i.inodeno == inode:
				used = True
		# unallocated nodes should be on free list
		if inode not in finodes and not used:
			print("UNALLOCATED INODE {} NOT ON FREELIST".format(inode))
			errs += 1
			unallocatedInodes.append(inode)

def audit_dirents():
	global errs

	linkMap = {}
	for dirent in dirents:
		# Directory entries should only refer to valid and allocated inodes, check validity of allocation status of referenced inodes
		if dirent.inode_num > ninodes:
			print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(dirent.parent, dirent.file_name, dirent.inode_num))
			errs += 1
		elif dirent.inode_num in unallocatedInodes:
			print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(dirent.parent, dirent.file_name, dirent.inode_num))
			errs += 1
		else:
			if dirent.inode_num not in linkMap:
				linkMap[dirent.inode_num] = 0
			linkMap[dirent.inode_num] = linkMap[dirent.inode_num] + 1

	for inode in allocatedInodes: 
		# Check if every inode is referred to by the number of directory tenries that is equal to reference count on inode
		if inode.inodeno in linkMap:
			if inode.link_count != linkMap[inode.inodeno]:
				print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(inode.inodeno, linkMap[inode.inodeno], inode.link_count))
				errs += 1
		elif inode.link_count != 0:
			print("INODE {} HAS 0 LINKS BUT LINKCOUNT IS {}".format(inode.inodeno, inode.link_count))
			errs += 1

	inodeToParent = {2: 2}  # include special root directory case

	# Check for correctness of itself and parent
	for dirent in dirents:
		if dirent.file_name != "'..'" and dirent.file_name != "'.'" and dirent.inode_num not in unallocatedInodes and dirent.inode_num <= ninodes:
			inodeToParent[dirent.inode_num] = dirent.parent
		elif dirent.file_name == "'.'":
			if dirent.inode_num != dirent.parent:
				print("DIRECTORY INODE {} NAME '.' LINK TO INODE {} SHOULD BE {}".format(dirent.parent, dirent.inode_num, dirent.parent))
				errs +=1
		elif dirent.file_name == "'..'" and dirent.inode_num != inodeToParent[dirent.parent]:
			print("DIRECTORY INODE {} NAME '..' LINK TO INODE {} SHOULD BE {}".format(dirent.parent, dirent.inode_num, inodeToParent[dirent.parent]))
			errs +=1

def main():
	if(len(sys.argv)) != 2:
		printExit("ERROR: Usage: ./lab3b FILENAME\n")

	# Read and parse file
	filename = sys.argv[1]
	if not os.path.isfile(filename):
		printExit("ERROR: " + filename + " does not exist\n")
	f = open(filename, 'r')
	if not f:
		printExit("ERROR: could not open file\n")
	if os.path.getsize(filename) <= 0:
		printExit("ERROR: empty file\n")

	parseCSV(f)
	audit_blocks()
	audit_inodes()
	audit_dirents()

	if errs != 0:
		exit(2)
	exit(0)

if __name__ == '__main__':
	main()