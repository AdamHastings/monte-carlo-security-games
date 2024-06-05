import plotly.graph_objects as go
import pandas as pd


def asset_flow_sankey(df):


  b = '#636EFA'
  y = '#FECB52'
  r = '#EF553B'
  g = '#888888'
  
  fig = go.Figure(data=[go.Sankey(
      node = dict(
        # pad = 15,
        thickness = 20,
        # line = dict(color = "black", width = 0.5),
        label = [
          "Defenders' initial wealth", # 0
          "Attackers' initial wealth", # 1
          "Insurers' initial wealth", # 2
          "Security spending", # 3
          "Ransom payments", # 4
          "Recovery costs", # 5
          "Premium pool", # 6
          "Claims", # 7
          "Attacker spending", # 8
          "Insurer spending", # 9
          "Defenders' final wealth", # 10
          "Attackers' final wealth", # 11
          "Insurers' final wealth" # 12
        ], 
        color = [b, r, y, g, r, g, y, y, g, g, b, r, y]
      ),
      link = dict(
        source = [0,          0,            0,          0,          0,          1,          1,  6, 2, 6, 7,  6,  4],
        target = [3,          4,            5,          6,          10,         8,          11, 9, 6, 7, 10, 12, 11],
        value =  [5466269888, 52736553959,  794751212,  306234183,  135567126,  8561332716, 3,  4, 4, 7, 7,  1,  52736553959],
        color =  [b, b, b, b, b,  r, r,  y, y, y, b,  y,  r]
    ))])

  fig.update_layout(title_text="Basic Sankey Diagram", font_size=10)
  fig.write_image("figures/asset_flow_sankey.pdf")
  fig.write_image("figures/asset_flow_sankey.png")


if __name__=="__main__":
    df = pd.read_csv("../logs/fullsize_short.csv", header=0)
    asset_flow_sankey(df)