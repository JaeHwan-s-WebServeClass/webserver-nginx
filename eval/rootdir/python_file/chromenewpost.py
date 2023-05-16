
import time

current_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

html="""

<!DOCTYPE html>
<html>
<head>
<link rel="icon" href="/favicon.png">
<title>Python CGI Page</title>
</head>
<body>
<b>server time : {}</b><br>
<b>text: aaaa</b><br>
</body>
</html>
""".format(current_time)

print(html)

