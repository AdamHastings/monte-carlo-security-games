import plotly.graph_objects as go
import pandas as pd



class flow:
  def __init__(self, source, sink, val, color):
    self.source = source
    self.sink = sink
    self.val = val
    self.color = color

opacity=0.6

b = 'rgba(99,  110, 250, {})'.format(opacity)
y = 'rgba(254, 203, 82,  {})'.format(opacity)
r = 'rgba(239, 85,  59,  {})'.format(opacity)
g = 'rgba(136, 136, 136, {})'.format(opacity)

opaque = 1

b0 = 'rgba(99,  110, 250, {})'.format(opaque)
y0 = 'rgba(254, 203, 82,  {})'.format(opaque)
r0 = 'rgba(239, 85,  59,  {})'.format(opaque)
g0 = 'rgba(136, 136, 136, {})'.format(opaque)

def asset_flow_sankey(df):


  nodes = {
      "Defenders' initial wealth" : b0,
      "Attackers' initial wealth" : r0,
      "Insurers' initial wealth" : y0,
      "Security spending" : b0,
      "Ransom payments" : b0,
      "Recovery costs" : b0,
      "Insurance premiums" : b0,
      "Premium pool" : y0,
      "Claims" : y0,
      "Attacker spending" : r0,
      "Insurer spending" : y0,
      "Defenders' final wealth" : b0,
      "Attackers' final wealth" : r0,
      "Insurers' final wealth" : y0,
      "Expenses" : g0
  }

  # nodemap
  nm = {k: v for v, k in enumerate(nodes.keys())}

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
    sink    = nm["Insurance premiums"],
    val     = 306234183,
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Insurance premiums"],
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
    color   = b)  
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
    color   = g)  
  flows.append(f)



  
  fig = go.Figure(go.Sankey(
      arrangement='snap',
      node = dict(
        pad = 15,
        thickness = 20,
        line = dict(color = "black", width = 0.5),
        label = list(nm.keys()),
        color = list(nodes.values())
      ),
      link = dict(
        # line = dict(color = "black", width = 0.5),
        source = [x.source for x in flows],
        target = [x.sink for x in flows],
        value  = [x.val for x in flows],
        color  = [x.color for x in flows]
      )
  ))

  fig.show()

  # fig.update_layout(title_text="Basic Sankey Diagram", font_size=10)
  fig.write_image("figures/asset_flow_sankey.pdf")
  fig.write_image("figures/asset_flow_sankey.png")


if __name__=="__main__":
    df = pd.read_csv("../logs/fullsize_short.csv", header=0)
    asset_flow_sankey(df)