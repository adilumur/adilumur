# Import required libraries
import pandas as pd
import dash
from dash import html
from dash import dcc
from dash.dependencies import Input, Output
import plotly.express as px

# Read the airline data into pandas dataframe
spacex_df = pd.read_csv("../data/launch_dash.csv")
max_payload = spacex_df['Payload Mass (kg)'].max()
min_payload = spacex_df['Payload Mass (kg)'].min()

# Create a dash application
app = dash.Dash(__name__)

# Create an app layout
app.layout = html.Div(children=[html.H1('SpaceX Launch Records Dashboard',
                                        style={'textAlign': 'center', 'color': '#503D36',
                                               'font-size': 40}),
                                # Dropdown list to enable Launch Site selection
                                html.Br(),
                                html.Div([# Two dropdown menus
                                        html.Label("Select Site"),
                                        dcc.Dropdown(
                                            id='site-dropdown',
                                            options=[
                                                {'label': 'All Sites', 'value': 'ALL'},
                                                {'label': 'CCAFS LC-40', 'value': 'CCAFS LC-40'},
                                                {'label': 'VAFB SLC-4E', 'value': 'VAFB SLC-4E'},
                                                {'label': 'KSC LC-39A', 'value': 'KSC LC-39A'},
                                                {'label': 'CCAFS SLC-40', 'value': 'CCAFS SLC-40'},
                                            ],
                                            value='ALL',
                                            placeholder="All sites",
                                            searchable=True
                                        )
                                    ],
                                        style = {'textAlign':'center', 
                                                'width':'80%',
                                                'padding':'3px', 
                                                'font-size':'20px',
                                                'margin':'auto'
                                                }
                                    ),
                                # Pie chart to show the total successful launches count for all sites
                                # If a specific launch site was selected, show the Success vs. Failed counts for the site
                                html.Div(dcc.Graph(id='success-pie-chart')),
                                html.Br(),

                                html.P("Payload range (Kg):"),
                                # Slider to select payload range
                                dcc.RangeSlider(id='payload-slider',
                                    min=0, max=10000, step=1000,
                                    marks={0: '0',
                                        2500: '2500',
                                        5000: '5000',
                                        7500: '7500',
                                        10000:'10000'},
                                    value=[min_payload, max_payload]
                                ),
                                # Scatter chart to show the correlation between payload and launch success
                                html.Div(dcc.Graph(id='success-payload-scatter-chart'))
                                ])


# Callback function for Pie Chart
@app.callback(Output(component_id='success-pie-chart', component_property='figure'),
              Input(component_id='site-dropdown', component_property='value'))
def get_pie_chart(entered_site):
    if entered_site == 'ALL':
        filtered_df = spacex_df.groupby('Launch Site')['class'].sum().reset_index()
        fig = px.pie(   
                        filtered_df, values='class', 
                        names='Launch Site', 
                        title='Total Success Launches By Site'
                    )
        return fig
    else:
        # return the outcomes piechart for a selected site
        filtered_df = spacex_df[spacex_df['Launch Site'] == entered_site]['class'].value_counts()
        fig = px.pie(
                        values = filtered_df.values,
                        names = filtered_df.index,
                        title = 'Success Launches vs Failure'
                    )
        return fig
    
# Callback function for Scatter plot
@app.callback(Output(component_id='success-payload-scatter-chart', component_property='figure'),
              [Input(component_id='site-dropdown', component_property='value'), 
               Input(component_id='payload-slider', component_property='value' )])
def get_pie_chart(entered_site, entered_payload):
    transformed_value = list(entered_payload)
    filtered = spacex_df[(spacex_df['Payload Mass (kg)'] >= transformed_value[0]) & (spacex_df['Payload Mass (kg)'] <= transformed_value[1])]
    if entered_site == 'ALL':
        fig = px.scatter(   
                        filtered, x='Payload Mass (kg)', 
                        y='class', color='Booster Version', 
                        title='Total Success Launches By Site'
                    )
        return fig
    else:
        fig = px.scatter(
                        filtered[filtered['Launch Site'] == entered_site],
                        x = 'Payload Mass (kg)',
                        y = 'class', color = 'Booster Version',
                        title = 'Success Launches vs Failure'
                    )
        return fig
    
# Run the app
if __name__ == '__main__':
    app.run_server()
