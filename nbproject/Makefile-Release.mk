#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Board.o \
	${OBJECTDIR}/_ext/home/zeroshade/Projects/Conn4/BitBoard.o \
	${OBJECTDIR}/core.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lncurses

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/conn4

dist/Release/GNU-Linux-x86/conn4: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/conn4 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Board.o: nbproject/Makefile-${CND_CONF}.mk Board.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/Board.o Board.cpp

${OBJECTDIR}/_ext/home/zeroshade/Projects/Conn4/BitBoard.o: nbproject/Makefile-${CND_CONF}.mk /home/zeroshade/Projects/Conn4/BitBoard.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/zeroshade/Projects/Conn4
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/home/zeroshade/Projects/Conn4/BitBoard.o /home/zeroshade/Projects/Conn4/BitBoard.cpp

${OBJECTDIR}/core.o: nbproject/Makefile-${CND_CONF}.mk core.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I/usr/include -MMD -MP -MF $@.d -o ${OBJECTDIR}/core.o core.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/conn4

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
