import plotly.graph_objects as go
import pandas as pd
import sys
import os
import os.path

## TODO this is wrong
## shows insurance claims as ending up in defenders' final wealth even when defenders are entirely looted...


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

  basetitle = 'asset_flow_sankey'
  dirname = 'figures'
  subdirname = df['folder'][0]

  df = df[[
            'd_init',
            'd_sum_security_investments',
            'attackerLoots',
            'a_end',
            'd_sum_recovery_costs',
            'sum_premiums_collected',
            'd_end',
            'a_init',
            'attackerExpenditures',
            'i_init',
            'paid_claims',
            'i_end',
          ]]

  meandf = df.mean()

  nodes = {
      "Defenders' initial wealth" : b0,
      "Defenders' post-spending wealth" : b0,
      "Attackers' initial wealth" : r0,
      "Attackers' post-ransom wealth" : r0,
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
    val     = meandf['d_sum_security_investments'],
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Security spending"],
    sink    = nm["Expenses"],
    val     = meandf['d_sum_security_investments'],
    color   = g)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' post-spending wealth"],
    sink    = nm["Ransom payments"],
    val     = meandf['attackerLoots'],
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Ransom payments"],
    sink    = nm["Attackers' post-ransom wealth"],
    val     = meandf['a_end'],
    color   = r)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' post-spending wealth"],
    sink    = nm["Recovery costs"],
    val     = meandf['d_sum_recovery_costs'],
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Recovery costs"],
    sink    = nm["Expenses"],
    val     = meandf['d_sum_recovery_costs'],
    color   = g)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' initial wealth"],
    sink    = nm["Insurance premiums"],
    val     = meandf['sum_premiums_collected'],
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' initial wealth"],
    sink    = nm["Defenders' post-spending wealth"],
    val     = (meandf['d_init'] - (meandf['sum_premiums_collected'] + meandf['d_sum_security_investments'])),
    color   = b)
  flows.append(f)

  f = flow(
    source  = nm["Attackers' initial wealth"],
    sink    = nm["Attackers' post-ransom wealth"],
    val     = meandf["a_init"],
    color   = r)
  flows.append(f)

  f = flow(
    source  = nm["Attackers' post-ransom wealth"],
    sink    = nm["Attackers' final wealth"],
    val     = meandf["a_end"],
    color   = r)
  flows.append(f)

  f = flow(
    source  = nm["Insurance premiums"],
    sink    = nm["Premium pool"],
    val     = meandf['sum_premiums_collected'],
    color   = y)  
  flows.append(f)

  # f = flow(
  #   source  = nm["Defenders' initial wealth"],
  #   sink    = nm["Defenders' final wealth"],
  #   val     = meandf['d_end'],
  #   color   = b)  
  # flows.append(f)

  f = flow(
    source  = nm["Ransom payments"],
    sink    = nm["Attacker spending"],
    val     = meandf['attackerExpenditures'],
    color   = r)  
  flows.append(f)

  # I worked on trying to split Attacker spending into splits from ransom and a_init. However, writing this out as a system of equations is underdetermined and so it can't be done.
  f = flow(
    source  = nm["Attacker spending"],
    sink    = nm["Expenses"],
    val     = meandf['attackerExpenditures'],
    color   = g)  
  flows.append(f)

  f = flow(
    source  = nm["Insurers' initial wealth"],
    sink    = nm["Premium pool"],
    val     = meandf['i_init'],
    color   = y)  
  flows.append(f)

  f = flow(
    source  = nm["Premium pool"],
    sink    = nm["Claims"],
    val     = meandf['paid_claims'],
    color   = y)  
  flows.append(f)

  f = flow(
    source  = nm["Claims"],
    sink    = nm["Defenders' post-spending wealth"],
    val     = meandf['paid_claims'],
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Defenders' post-spending wealth"],
    sink    = nm["Defenders' final wealth"],
    val     = meandf['d_end'],
    color   = b)  
  flows.append(f)

  f = flow(
    source  = nm["Premium pool"],
    sink    = nm["Insurers' final wealth"],
    val     = meandf['i_end'],
    color   = y)  
  flows.append(f)

  # f = flow(
  #   source  = nm["Premium pool"],
  #   sink    = nm["Insurer spending"],
  #   val     = meandf['insurer_expenditures'],
  #   color   = y)  
  # flows.append(f)

  # f = flow(
  #   source  = nm["Insurer spending"],
  #   sink    = nm["Expenses"],
  #   val     = meandf['insurer_expenditures'],
  #   color   = g)  
  # flows.append(f)



  
  fig = go.Figure(go.Sankey(
      arrangement='snap',
      node = dict(
        pad = 50,
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

  # fig.show()
  path = dirname + '/' + subdirname 
  
  if not os.path.isdir(path):
      os.mkdir(path)

  # fig.update_layout(title_text="Basic Sankey Diagram", font_size=10)
  fig.write_image(path + '/' + basetitle + '.png')
  fig.write_image(path + '/' + basetitle + '.pdf')

  


if __name__=="__main__":

  if (len(sys.argv) == 2):
    filename = sys.argv[1]
  else:
    print("Incorrect number of args! Example:")
    print("$ python3 run_all.py <path_to_log_file.csv>")
    sys.exit(1)

  df = pd.read_csv(filename, header=0)
  filename = filename.replace("../logs/", "")
  filename = filename.replace(".csv", "")
  df['folder'] = filename

  asset_flow_sankey(df)