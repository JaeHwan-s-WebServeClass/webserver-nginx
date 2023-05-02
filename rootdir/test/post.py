import cgi
import time

print("Content-type: text/html\r")
print("\r")
print("<!DOCTYPE html>")
print("<html>")
print('<head>')
print('<title>Python CGI Page</title>')
print('</head>')
print("<body>")
print("")
print("<b>server time : " + str(time.time()) + "</b><br>")
print("<form method='get' action='pycgi.py'>")
print("  Name: <input type='text' name='fname'>")
print("  <input type='submit'>")
print("</form>")
print("")

form = cgi.FieldStorage()
if "fname" not in form:
    print("<b>data query is not exist!</b>")
else:
    print("<b>data : " + form["fname"].value + "</b>")
print("<br>")
print("<br>")
print("<b>CGI output Delay Test</b>")
print("<br>")
for i in range(1, 2):
    print("<b>")
    print(i)
    print("s</b>")
    print("<br>")
    time.sleep(1)

print("")
print("</body>")
print("</html>")