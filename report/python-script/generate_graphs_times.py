import pandas as pd
import plotly.graph_objects as go
import os
import glob

def generate_graphs():
    # Define paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(script_dir, '../data')
    img_dir = os.path.join(script_dir, '../img')

    # Ensure output directory exists
    if not os.path.exists(img_dir):
        os.makedirs(img_dir)

    # Find all CSV files
    csv_files = glob.glob(os.path.join(data_dir, '*.csv'))

    if not csv_files:
        print(f"No CSV files found in {data_dir}")
        return

    for csv_file in csv_files:
        filename = os.path.basename(csv_file)
        print(f"Processing {filename}...")
        
        try:
            df = pd.read_csv(csv_file)
            
            # Check required columns
            required_columns = {'method', 'nb_proc', 'time'}
            if not required_columns.issubset(df.columns):
                print(f"Skipping {filename}: Missing required columns {required_columns - set(df.columns)}")
                continue

            # Create figure
            fig = go.Figure()

            # Get sequential time (baseline)
            seq_data = df[df['method'] == 'sequential']
            if not seq_data.empty:
                # Assuming one sequential entry per file or taking the average if multiple (though usually one for a specific size)
                # If there are multiple sizes, we might need to handle that, but the prompt implies "one graph per file" and files seem to be split by problem type.
                # However, looking at the data, it has 'size' column. If size varies, we might have an issue.
                # The user said "trace le graphe avec en absice le nombre de proc et en ordonée le temps".
                # Let's assume the file contains data for a single problem size or we just plot everything.
                # Based on the file content view earlier: "matrix_power2.csv" has size 100000000 for all rows.
                # So we can take the first sequential time found.
                seq_time = seq_data['time'].iloc[0]
                
                # Add sequential line
                # We want it to span the range of nb_proc
                max_proc = df['nb_proc'].max()
                min_proc = df[df['method'] != 'sequential']['nb_proc'].min()
                if pd.isna(min_proc): min_proc = 1 # Fallback if only sequential exists
                
                fig.add_trace(go.Scatter(
                    x=[min_proc, max_proc],
                    y=[seq_time, seq_time],
                    mode='lines',
                    name='Sequential',
                    line=dict(color='red', dash='dash')
                ))
            
            # Plot other methods
            methods = df['method'].unique()
            for method in methods:
                if method == 'sequential':
                    continue
                
                method_data = df[df['method'] == 'method'].sort_values('nb_proc') # Wait, this is a bug in my thought, should be variable `method`
                method_data = df[df['method'] == method].sort_values('nb_proc')
                
                fig.add_trace(go.Scatter(
                    x=method_data['nb_proc'],
                    y=method_data['time'],
                    mode='lines+markers',
                    name=method
                ))

            # Update layout
            title = filename.replace('.csv', '').replace('_', ' ').title()
            fig.update_layout(
                title=f'Performance: {title}',
                xaxis_title='Number of Processes',
                yaxis_title='Time (s)',
                template='plotly_white'
            )

            # Save as PNG
            output_path = os.path.join(img_dir, filename.replace('.csv', '.png'))
            fig.write_image(output_path, scale=3)
            print(f"Saved graph to {output_path}")

        except Exception as e:
            print(f"Error processing {filename}: {e}")

if __name__ == "__main__":
    generate_graphs()
