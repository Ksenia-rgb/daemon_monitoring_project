#!/usr/bin/env python3

import re
import sys

def check_pr_title(title: str) -> re.match:
  if not title:
    return None
  match = re.search(r'^\[(KS|IL|MI|ST|MA)-(D|C)\]', title)
  if not match:
    return re.search(r'^\[KS-(master|tests)\]', title)

def main():
  #title = str(os.getenv("PR_TITLE"))
  
  if len(sys.argv) != 2:
    print("Incorrect script arguments")
    return 1
  title = sys.argv[1]
  
  if not check_pr_title(title):
    print("Pr title must begin with your [init letters] and continue with [C for Client] or [D for Daemon] or [master] or [tests]")
    return 1
  print("Pr title is correct")


main()