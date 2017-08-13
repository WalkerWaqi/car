import pexpect
import types, paramiko, os, sys, re, time, getpass
from sys import argv


def sshclient_execmd(hostname, port, username, password, execmd, exein):
    result = ""
    try:
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(hostname, port, username, password)
        channel = ssh.invoke_shell()
        print "run: ", execmd
        channel.send(execmd)
        if exein:
            while True:
                result = channel.recv(1024)
                print result
                if re.search(".*\[sudo\].*", result):
                    break;
                    time.sleep(1)
                time.sleep(1)
        else:
            time.sleep(1)
        for k, v in enumerate(exein):
            print "write:", v
            channel.send(v)
            result = ''
            while not result:
                time.sleep(1)
                result = channel.recv(1024)
            print result
        ssh.close()
    except:
        print "sshclient_execmd err."
        raise Exception("sshclient_execmd err.")
    return result


def ssh_command(user, host, password, command):
    ssh_newkey = '.*(yes/no).*'
    passwd_key = '.*assword.*'
    child = pexpect.spawn('ssh -l %s %s %s' % (user, host, command))
    child.logfile = sys.stdout
    i = child.expect([pexpect.TIMEOUT, ssh_newkey, passwd_key])

    if i == 0:  # timeout
        print child.before
        print "Error time out"
        print child.after
        return None
    if i == 1:
        child.sendline('yes')
        i = child.expect([pexpect.TIMEOUT, passwd_key])
        if i == 0:
            print child.before
            print 'time out ERROR'
            print child.after
            return None
    child.sendline(password)
    return child


def scp2(ip, user, passwd, filename, dst_path):
    passwd_key = '.*assword.*'
    if os.path.isdir(filename):
        cmdline = 'scp -r %s %s@%s:%s' % (filename, user, ip, dst_path)
    else:
        cmdline = 'scp %s %s@%s:%s' % (filename, user, ip, dst_path)
    try:
        print "uploading " + filename
        child = pexpect.spawn(cmdline)
        child.expect(passwd_key)
        child.sendline(passwd)
        child.expect(pexpect.EOF)
        # child.interact()
        # child.read()
        # child.expect('$')
        print "uploaded " + filename
    except:
        print "upload faild!"
        raise Exception("upload faild!")


def main():
    script, hostname = argv
    port = 22
    username = 'walker'
    password = 'Qwe123!@#'
    install_path = 'bin'
    target = '/opt/bin/'

    execmds = [{'mkdir -p /home/' + username + '/' + install_path +'\n': []}]
    for k, v in enumerate(execmds):
        for kk in v:
            vv = v.get(kk)
            sshclient_execmd(hostname, port, username, password, kk, vv)

    scp2(hostname, username, password, './car', '/home/' + username + '/' + install_path)
    scp2(hostname, username, password, './car.lgp', '/home/' + username + '/' + install_path)
    scp2(hostname, username, password, './car.service', '/home/' + username + '/' + install_path)
    scp2(hostname, username, password, './cardump.service', '/home/' + username + '/' + install_path)
    scp2(hostname, username, password, './data', '/home/' + username + '/' + install_path)

    execmds = [{'sudo mkdir -p ' + target + '/logger\n': [password +'\n']},
               {'sudo systemctl stop car.service\n': [password +'\n']},
               {'sudo systemctl stop cardump.service\n': [password +'\n']},
               {'sudo cp /home/' + username + '/' + install_path + '/car ' + target + '\n': [password +'\n']},
               {'sudo cp /home/' + username + '/' + install_path + '/car.lgp ' + target + '\n': [password + '\n']},
               {'sudo cp /home/' + username + '/' + install_path + '/car.service /lib/systemd/system/\n': [password +'\n']},
               {'sudo cp /home/' + username + '/' + install_path + '/cardump.service /lib/systemd/system/\n': [password +'\n']},
               {'sudo chmod +x ' + target + '/car' + '\n': [password + '\n']},
               {'sudo systemctl enable car.service\n': [password +'\n']},
               {'sudo systemctl enable cardump.service\n': [password +'\n']},
               {'sudo reboot\n': [password +'\n']}]
    for k, v in enumerate(execmds):
        for kk in v:
            vv = v.get(kk)
            sshclient_execmd(hostname, port, username, password, kk, vv)


if __name__ == "__main__":
    main()
