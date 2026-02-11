#!/usr/bin/env python3

import re
import os
import sys
import json

def check_commit_message(msg: str) -> bool:
  if not msg:
    return False
  if re.search(r'^Merge', msg):
    return True
  match = re.search(r'^\[(KS|IL|MI|ST|MA)-(D|C)\]', msg)
  if not match:
    return re.search(r'^\[KS-(master|tests)\]', msg)

def main():
  commits = os.getenv("COMMITS")
  if not commits:
    print("Incorrect script arguments")
    sys.exit(1)
  
  messages = [entry["message"] for entry in json.loads(commits)]
  for msg in messages:
    if not check_commit_message(msg):
      print("Commit message must begin with your [init letters] and continue with [C for Client] or [D for Daemon] or [master] or [tests]")
      print("Your current message:", msg)
      sys.exit(1)
  print("All commits messages are correct")
  

main()