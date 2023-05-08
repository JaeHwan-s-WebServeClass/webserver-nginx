import time
import sys
import cgi

argument1 = sys.argv[1]

# 쿼리 스트링을 & 기준으로 쪼개서 리스트로 저장
query_list = argument1.split("&")

# 각 쿼리를 다시 = 기준으로 쪼개서 딕셔너리에 저장
query_dict = {}
for query in query_list:
    key, value = query.split("=")
    query_dict[key] = value

def generate_python_code(text):
    code = """
import time

current_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

html=""" + '\"\"\"\n' + """
<!DOCTYPE html>
<html>
<head>
<title>Python CGI Page</title>
</head>
<body>
<b>server time : {}</b><br>
<b>text: """ + text + """</b><br>
</body>
</html>
\"\"\".format(current_time)

print(html)
"""
    return code
print(generate_python_code(query_dict["text"]))
