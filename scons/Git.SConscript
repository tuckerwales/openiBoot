import subprocess

def GetGitCommit():
	p = subprocess.Popen('git log | head -n1 | cut -b8-14', shell=True, stdout=subprocess.PIPE)
	ret, smth = p.communicate()
	if isinstance(ret, bytes):
		ret = ret.decode('utf-8')
	return ret.rstrip('\n')
Export('GetGitCommit')
