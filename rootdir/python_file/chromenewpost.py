
import time

current_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

html="""

<!DOCTYPE html>
<html>
<head>
<title>Python CGI Page</title>
</head>
<body>
<b>server time : {}</b><br>
<b>text: abcd</b><br>
</body>
</html>
""".format(current_time)

print(html)

