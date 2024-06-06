import plotly.graph_objects as go
import pandas as pd



class flow:
  def __init__(self, source, sink, val, color):
    self.source = source
    self.sink = sink
    self.val = val
    self.color = color

b = '#636EFA'
y = '#FECB52'
r = '#EF553B'
g = '#888888'

def asset_flow_sankey(df):


  nodes = [
      "Defenders' initial wealth",
      "Attackers' initial wealth",
      "Insurers' initial wealth",
      "Security spending",
      "Ransom payments",
      "Recovery costs",
      "Premium pool",
      "Claims",
      "Attacker spending",
      "Insurer spending",
      "Defenders' final wealth", 
      "Attackers' final wealth", 
      "Insurers' final wealth", 
      "Expenses"
  ]

  # nodemap
  nm = {k: v for v, k in enumerate(nodes)}

  print(nm)

  flows = []

  f = flow(
    source  = nm["Defenders' initial wealth"],
    sink    = nm["Security spending"],
    val     = 5466269888,
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Security spending"],
    sink    = nm["Expenses"],
    val     = 5466269888,
    color   = g)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' initial wealth"],
    sink    = nm["Ransom payments"],
    val     = 52736553959,
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Ransom payments"],
    sink    = nm["Attackers' final wealth"],
    val     = 44175509863,
    color   = r)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' initial wealth"],
    sink    = nm["Recovery costs"],
    val     = 794751212,
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Recovery costs"],
    sink    = nm["Expenses"],
    val     = 794751212,
    color   = g)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' initial wealth"],
    sink    = nm["Premium pool"],
    val     = 306234183,
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' initial wealth"],
    sink    = nm["Defenders' final wealth"],
    val     = 135567126,
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Attackers' initial wealth"],
    sink    = nm["Ransom payments"],
    val     = 288620,
    color   = r)  
  flows.append(f)

  f = flow(
    source  = nm["Ransom payments"],
    sink    = nm["Attacker spending"],
    val     = 8561332716,
    color   = r)  
  flows.append(f)

  f = flow(
    source  = nm["Attacker spending"],
    sink    = nm["Expenses"],
    val     = 8561332716,
    color   = g)  
  flows.append(f)

  f = flow(
    source  = nm["Insurers' initial wealth"],
    sink    = nm["Premium pool"],
    val     = 2641561568,
    color   = y)  
  flows.append(f)

  f = flow(
    source  = nm["Premium pool"],
    sink    = nm["Claims"],
    val     = 311355971,
    color   = y)  
  flows.append(f)

  f = flow(
    source  = nm["Claims"],
    sink    = nm["Defenders' final wealth"],
    val     = 311355971,
    color   = y)  
  flows.append(f)

  f = flow(
    source  = nm["Premium pool"],
    sink    = nm["Insurers' final wealth"],
    val     = 2569925532,
    color   = y)  
  flows.append(f)

  f = flow(
    source  = nm["Premium pool"],
    sink    = nm["Insurer spending"],
    val     = 66514248,
    color   = y)  
  flows.append(f)

  f = flow(
    source  = nm["Insurer spending"],
    sink    = nm["Expenses"],
    val     = 66514248,
    color   = y)  
  flows.append(f)



  
  fig = go.Figure(data=[go.Sankey(
      arrangement='perpendicular',
      node = dict(
        # pad = 15,
        thickness = 20,
        # line = dict(color = "black", width = 0.5),
        label = list(nm.keys())
        # color = [b, r, y, g, r, g, y, y, g, g, b, r, y]
      ),
      link = dict(
        # source = [0, 0, 0, 0, 0,  1, 1,  6, 2, 6, 7,  6,  4],
        # target = [3, 4, 5, 6, 10, 8, 11, 9, 6, 7, 10, 12, 11],
        # value =  [8, 4, 2, 8, 4,  2, 3,  4, 4, 7, 7,  1,  4],
        # color =  [b, b, b, b, b,  r, r,  y, y, y, b,  y,  r]
        source = [x.source for x in flows],
        target = [x.sink for x in flows],
        value  = [x.val for x in flows],
        color  = [x.color for x in flows]
    ))])

  fig.update_layout(title_text="Basic Sankey Diagram", font_size=10)
  fig.write_image("figures/asset_flow_sankey.pdf")
  fig.write_image("figures/asset_flow_sankey.png")


if __name__=="__main__":
    df = pd.read_csv("../logs/fullsize_short.csv", header=0)
    asset_flow_sankey(df)