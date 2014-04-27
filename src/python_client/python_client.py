
#address = os.path.abspath(os.path.join(os.curdir, os.pardir))
#os.environ['PATH'] = ';'.join([address + '\\Win32\\Debug', os.environ['PATH']])

with open('..\\..\\datasets\\vocab.kos.txt', 'r') as content_file:
    content = content_file.read()

tokens = content.split('\n')

print 'done'
