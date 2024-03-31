git add .
git commit -m "$(date)"
git pull && git push && python3 checkin.py
