from flask import Flask, render_template
import csv

app = Flask(__name__)

def read_data():
    rides = []
    total_rides = 0
    total_revenue = 0

    try:
        with open('../data/data.csv', 'r') as file:
            reader = csv.reader(file)
            for row in reader:
                rides.append(row)
                total_rides += 1
                total_revenue += float(row[5])
    except:
        pass

    return rides, total_rides, total_revenue


@app.route('/')
def home():
    rides, total_rides, total_revenue = read_data()
    return render_template("home.html",
                           total_rides=total_rides,
                           total_revenue=total_revenue)


@app.route('/data')
def data():
    rides, _, _ = read_data()
    return render_template("data.html", rides=rides)


@app.route('/stats')
def stats():
    _, total_rides, total_revenue = read_data()
    return render_template("stats.html",
                           total_rides=total_rides,
                           total_revenue=total_revenue)


app.run(debug=True)