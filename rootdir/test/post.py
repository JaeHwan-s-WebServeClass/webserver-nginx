import sys
import cgi
import time

argument1 = sys.argv[1]
# print("The argument1 is:", argument1)


# 쿼리 스트링을 & 기준으로 쪼개서 리스트로 저장
query_list = argument1.split("&")

# 각 쿼리를 다시 = 기준으로 쪼개서 딕셔너리에 저장
query_dict = {}
for query in query_list:
    key, value = query.split("=")
    query_dict[key] = value

# 결과 출력
# print(query_dict)

html = """
<!DOCTYPE html>
<html>
<head>
<title>Python CGI Page</title>
</head>
<body>
<b>server time : {}</b><br>
<b>{}</b><br>
</body>
</html>
""".format(str(time.time()), query_dict["text"])

print(html)

# print("<!DOCTYPE html>")
# print("<html>")
# print('<head>')
# print('<title>Python CGI Page</title>')
# print('</head>')
# print("<body>")
# print("")
# print("<b>server time : " + str(time.time()) + "</b><br>")
# print("<form method='POST' action='post.py'>")
# print("  Name: <input type='text' name='name'>")
# print("  Email: <input type='text' name='email'>")
# print("  <input type='submit'>")
# print("</form>")
# print("")
# print("")
# print("</body>")
# print("</html>") 