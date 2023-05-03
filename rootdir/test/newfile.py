import time

html="""

<!DOCTYPE html>
<html>
<head>
<title>Python CGI Page</title>
</head>
<body>
<b>server time : {}</b><br>
<b>python file for get method</b><br>
</body>
</html>
""".format(str(time.time()))

print(html)