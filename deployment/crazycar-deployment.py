import os
import sys
import stat
import serial
import shutil
import subprocess
from sys import platform
from urllib.request import urlopen
from PyQt5 import QtGui, QtSerialPort
from PyQt5.QtGui import *
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *

#
# https://github.com/loharders/CrazyCar-Deployment.git
#

class USBProgrammer:

    # Construktor
    def __init__(self):

        # Initialize variable
        self.avr_handle = None

        # Check available ports
        self._ports = USBProgrammer.checkAvailablePorts()

    # Destruktor
    def __del__(self):

        try:

            # Kill process
            self.avr_handle.kill()

        except:
            pass

    def runWinAVR(self, path):

        # Run WinAVR
        self.avr_handle = subprocess.Popen([path], shell=True)

    @staticmethod
    def checkAvailablePorts():

        # Variable zum Speichern der Ports und Ergebnisse initialisieren.
        ports = []
        result = []

        # Prüfe auf Windows-Umgebung.
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]

        # Prüfe auf Linux-Umgebung.
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            temp = list(list_ports.comports())
            for dev in temp:
                ports.append(dev.device)

        # Prüfe auf Darwin-Umgebung.
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')

        # Fehler werfen, falls es nicht um ein Windows, Linux oder Darwin-System handelt.
        else:
            raise EnvironmentError('Unsupported platform')

        # Die Liste von Ports durchgehen und testweise verbinden.
        # Nur Ports mit denen sich erfolgreich verbunden wurde, werden weiterverwendet.
        for port in ports:
            try:

                # Serielle Verbindung aufbauen.
                s = serial.Serial(port, 19200)

                # Verbindung schließen.
                s.close()

                # Port in der Ergebnisliste speichern.
                result.append(port)

            except (OSError, serial.SerialException):
                pass

        # Ergebnisliste zurückgeben
        return result

    @staticmethod
    def changeWorkingDir(path: str):

        # Change working directory
        os.chdir(path)

    @staticmethod
    def checkForOutputErrors(output: str):

        # Check if error terms are in the string
        if "Error" in output or "error" in output or "failed" in output:
            return True
        else:
            return False

    @staticmethod
    def changePortInMakefile(mfile: str, port: str):

        # Define variable to save the makefile lines
        newlines = []

        try:

            # Read in the file
            file = open(mfile, 'r')
            filedata = file.readlines()
            file.close()

            # Go through the file line by line
            for l in filedata:

                # Find line that defines the port
                if "AVRDUDE_PORT" in l:

                    # Add line
                    newlines.append("AVRDUDE_PORT = " + port)

                else:

                    # Add line
                    newlines.append(l)

            # Write the file out again
            file = open(mfile, 'w')
            file.writelines(newlines)
            file.close()

            return 0, ''

        except Exception as e:

            return 1, e

    @staticmethod
    def clean():

        # Clean the project
        clean_result = subprocess.run(['make', 'clean'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # Return process output
        return clean_result.stdout.decode('utf-8'), clean_result.stderr.decode('utf-8')

    @staticmethod
    def make():

        # Make the project
        make_result = subprocess.run(['make', 'all'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

        # Return process output
        return make_result.stdout.decode('utf-8'), make_result.stderr.decode('utf-8')

    @staticmethod
    def program():

        try:

            # Program the board
            program_result = subprocess.run(['make', 'program'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

            # Return process output
            return 0, program_result.stdout.decode('utf-8'), program_result.stderr.decode('utf-8')

        except Exception as e:

            return 1, '', e


class GitLoad:

    @staticmethod
    def deleteFolder(path: str):

        try:

            # Check if the folder exists
            if os.path.isdir(path) and os.path.exists(path):
                for root, dirs, files in os.walk(path, topdown=False):
                    for name in files:
                        filename = os.path.join(root, name)
                        os.chmod(filename, stat.S_IWUSR)
                        os.remove(filename)
                    for name in dirs:
                        os.rmdir(os.path.join(root, name))
                os.rmdir(path)

            return 0, ''

        except Exception as e:
            return 1, e

    @staticmethod
    def downloadBranch(repo: str, target: str, branch: str = 'main'):

        # Delete folder
        state, err = GitLoad.deleteFolder(target)
        print("deleteFolder: ", state)
        print("Message: ", err)

        # Prüfen, ob der Ordner bereits existiert.
        if not os.path.exists(target):

            # Create folder
            os.mkdir(target)

        # Clone repository
        branch_result = subprocess.run(['git', 'clone', '--branch', branch, repo, target], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

        # Return resulting outputs
        return branch_result.stdout.decode('latin').strip('\r\n'), branch_result.stderr.decode('latin').strip('\r\n')

    @staticmethod
    def getBranches(repo: str):

        # Initialize variable
        result = []

        try:
            branch_result = subprocess.run(['git', 'ls-remote', '--heads', repo], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
            branches = branch_result.stdout.decode('latin').split('\n')
            for b in branches:
                if len(b) == 0:
                    continue
                result.append(b[b.find('heads')+6:])
            return result
        except:
            return result


class ConfigWindow(QWidget):

    # Klassenkonstruktor.
    def __init__(self):

        # Konstruktor der Basisklasse aufrufen.
        super().__init__()

        # Fenstertitel festlegen.
        self.isConnected = None
        self.setFixedWidth(600)
        self.setFixedHeight(580)
        self.setWindowTitle("CrazyCar - Deployment")

        # Variablen initialisieren.
        self._made = False
        self._programmer = USBProgrammer()

        # Logo-Label erstellen.
        self.labelLogo = QLabel(self)
        self.labelLogo.setAlignment(Qt.AlignTop | Qt.AlignCenter)

        self.labelPort = QLabel(self)
        self.labelPort.setMaximumSize(120, self.labelPort.height())
        self.labelPort.setText("Programmieradapter: ")
        self.labelPort.setAlignment(Qt.AlignVCenter | Qt.AlignLeft)
        self.selectPort = QComboBox()
        self.selectPort.addItems(["Wähle Port ..."])

        self.labelRepo = QLabel(self)
        self.labelRepo.setMaximumSize(120, self.labelRepo.height())
        self.labelRepo.setText("Repository: ")
        self.labelRepo.setAlignment(Qt.AlignVCenter | Qt.AlignLeft)
        self.inputRepo = QLineEdit()
        # self.inputRepo.setText('https://github.com/loharders/CrazyCar-Deployment.git')

        self.labelBranch = QLabel(self)
        self.labelBranch.setMaximumSize(120, self.labelBranch.height())
        self.labelBranch.setText("Branch: ")
        self.labelBranch.setAlignment(Qt.AlignVCenter | Qt.AlignLeft)
        self.selectBranch = QComboBox()
        self.selectBranch.addItems(["Wähle Branch ..."])

        self.buttonCompile = QPushButton()
        self.buttonProgram = QPushButton()
        self.buttonCompile.setText("Kompilieren")
        self.buttonProgram.setText("Programmieren")

        self.output = QTextEdit()
        self.output.setEnabled(True)
        self.output.setReadOnly(True)
        self.output.setMinimumHeight(350)
        self.output.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

        ###

        mainLayout = QVBoxLayout()

        # 1. Groupbox - Programmieradapter
        gbox1 = QGroupBox("Programmierport")
        hlayout1 = QHBoxLayout()
        hlayout1.addWidget(self.labelPort)
        hlayout1.addWidget(self.selectPort)
        gbox1.setLayout(hlayout1)
        mainLayout.addWidget(gbox1)

        # 2. Groupbox - GitHub
        gbox3 = QGroupBox("Github")
        hlayout3 = QHBoxLayout()
        hlayout4 = QHBoxLayout()
        vlayout1 = QVBoxLayout()
        hlayout3.addWidget(self.labelRepo)
        hlayout3.addWidget(self.inputRepo)
        hlayout4.addWidget(self.labelBranch)
        hlayout4.addWidget(self.selectBranch)
        vlayout1.addLayout(hlayout3)
        vlayout1.addLayout(hlayout4)
        gbox3.setLayout(vlayout1)
        mainLayout.addWidget(gbox3)

        # Spacer hinzufügen.
        mainLayout.addItem(QSpacerItem(20, 10, QSizePolicy.Minimum, QSizePolicy.Expanding))

        # 2. Groupbox - Renneinstellungen
        gbox2 = QGroupBox("")
        hlayout2 = QHBoxLayout()
        hlayout2.addWidget(self.buttonCompile)
        hlayout2.addWidget(self.buttonProgram)
        gbox2.setLayout(hlayout2)
        mainLayout.addWidget(gbox2)

        # Spacer hinzufügen.
        mainLayout.addItem(QSpacerItem(10, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))

        # Add output
        mainLayout.addWidget(self.output)

        self.setLayout(mainLayout)

    def updateBranches(self, inpt: str):

        # Check if the name ends with a valid git file type
        if inpt.endswith(".git"):

            # override cursor
            QApplication.setOverrideCursor(Qt.WaitCursor)

            try:

                # Get branches
                branches = GitLoad.getBranches(repo=inpt)

                # Check return value
                if branches:

                    # Clear the selection
                    self.selectBranch.clear()

                    # Add the items to the selection input
                    self.selectBranch.addItems(branches)

            finally:

                # Restore cursor
                QApplication.restoreOverrideCursor()

    def make(self):

        # override cursor
        QApplication.setOverrideCursor(Qt.WaitCursor)

        try:

            # Check selected branch
            if 'Wähle' in self.selectBranch.currentText():
                self.output.append("Es wurde kein gültiges Repository ausgewählt.\n")
                return 1
            else:
                self.output.clear()

            # Show message
            self.output.append("Starte Kompilierung ...")

            # Check OS type and determine path to destop
            if platform == "linux" or platform == "linux2":
                self.output.append("Linux-Umgebung erkannt ...\n")
                desktop = os.path.join(os.path.join(os.path.expanduser('~')), 'Desktop')
            elif platform == "win32":
                self.output.append("Windows-Umgebung erkannt ...\n")
                desktop = os.path.join(os.path.join(os.environ['USERPROFILE']), 'Desktop')
            else:
                self.output.append("Not a known system.\n")
                self.output.append("Abbruch.\n")
                return 1

            # Define target folder name
            target = desktop + '\\test'

            # Start WinAVR
            if self._programmer.avr_handle is not None:

                # Prüfen, ob das Programm bereits läuft.
                poll = self._programmer.avr_handle.poll()
                if poll is None:
                    print("Kill ...")
                    self._programmer.avr_handle.kill()

                # Programm starten.
                self._programmer.runWinAVR('C:\\WinAVR-20100110\\utils\\bin\\sh.exe')

            else:
                self._programmer.runWinAVR('C:\\WinAVR-20100110\\utils\\bin\\sh.exe')

            # Load Branch
            _, _ = GitLoad.downloadBranch(repo='https://github.com/loharders/CrazyCar-Deployment.git', branch=self.selectBranch.currentText(), target=target)

            # Change working directory
            self._programmer.changeWorkingDir(target)

            # Clean the project
            c_std, c_err = self._programmer.clean()
            if USBProgrammer.checkForOutputErrors(c_err):
                self.output.append("Es ist ein Fehler bei der Bereinigung des Projekts aufgetretenError cleaning the project:")
                self.output.append(c_err)
                return 1
            else:
                self.output.append(c_std)

            # Make the project
            m_std, m_err = self._programmer.make()
            if USBProgrammer.checkForOutputErrors(m_err):
                self.output.append("Es ist ein Fehler bei der Kompilierung aufgetreten: ")
                self.output.append(m_err)
                return 1
            else:
                self.output.append(m_std)
                self.output.append("\n")
                self.output.append("Kompilierung erfolgreich abgeschlossen!")
                self._made = True

        finally:

            # Restore cursor
            QApplication.restoreOverrideCursor()

    def program(self):

        # override cursor
        QApplication.setOverrideCursor(Qt.WaitCursor)

        try:

            if not self._made:

                # Show error
                self.output.append("Es ist ein Fehler aufgetreten:\n")
                self.output.append("Sie müssen das Programm zunächst kompilieren.")
                return

            # Port abfragen
            if "Wähle" in self.selectPort.currentText():

                # Show error
                self.output.append("Es ist ein Fehler aufgetreten:\n")
                self.output.append("Sie müssen ein Port auswählen.")
                return

            # Change port
            state, err = USBProgrammer.changePortInMakefile('Makefile', self.selectPort.currentText().lower())
            if state != 0:

                # Show error
                self.output.append("Es ist ein Fehler bei Anpassung des Ports im Makefile aufgetreten.\n")
                self.output.append("Fehler: " + str(err))
                return

            # Program the project
            state, p_std, p_err = self._programmer.program()

            if state == 0:
                if USBProgrammer.checkForOutputErrors(p_err):
                    self.output.append("Es ist ein Fehler bei der Programmierung aufgetreten: ")
                    self.output.append(p_err)
                else:
                    self.output.append(p_std)
                    self.output.append(p_err)
                    self._made = False
            else:
                self.output.append("Es ist ein Fehler bei der Programmierung aufgetreten: ")
                self.output.append(str(p_err))

        finally:

            # Restore cursor
            QApplication.restoreOverrideCursor()


def main():

    # Starte die Hauptanwendung mit den übergebenen Parametern.
    app = QApplication(sys.argv)

    # Initialisiere des Anwendungsfensters.
    config = ConfigWindow()

    # Signale verbinden.
    config.inputRepo.textChanged.connect(config.updateBranches)
    config.selectPort.addItems(USBProgrammer.checkAvailablePorts())
    config.buttonCompile.clicked.connect(config.make)
    config.buttonProgram.clicked.connect(config.program)

    # Zeige das Hauptfenster.
    config.show()

    # Starte die Anwendung.
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
