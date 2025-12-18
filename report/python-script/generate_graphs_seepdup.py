import pandas as pd
import plotly.graph_objects as go
import os
import glob

def generate_speedup_graphs():
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

            # Get sequential time (baseline for speedup calculation)
            seq_data = df[df['method'] == 'sequential']
            if seq_data.empty:
                print(f"Skipping {filename}: No sequential data found for speedup calculation")
                continue

            seq_time = seq_data['time'].iloc[0]

            # Add ideal speedup line (speedup = nb_proc)
            max_proc = df['nb_proc'].max()
            min_proc = df[df['method'] != 'sequential']['nb_proc'].min()
            if pd.isna(min_proc):
                min_proc = 1

            fig.add_trace(go.Scatter(
                x=[min_proc, max_proc],
                y=[min_proc, max_proc],
                mode='lines',
                name='Speedup Idéal',
                line=dict(color='black', dash='dash')
            ))

            # Calculate and plot speedup for other methods
            methods = df['method'].unique()
            for method in methods:
                if method == 'sequential':
                    continue

                method_data = df[df['method'] == method].sort_values('nb_proc')

                # Calculate speedup: T_sequential / T_parallel
                speedup = seq_time / method_data['time']

                fig.add_trace(go.Scatter(
                    x=method_data['nb_proc'],
                    y=speedup,
                    mode='lines+markers',
                    name=method
                ))

            # Update layout
            title = filename.replace('.csv', '').replace('_', ' ').title()
            fig.update_layout(
                title=f'Speedup: {title}',
                xaxis_title='Nombre de Processus',
                yaxis_title='Speedup (T_seq / T_parallel)',
                template='plotly_white',
                legend=dict(
                    yanchor="top",
                    y=0.99,
                    xanchor="left",
                    x=0.01
                )
            )

            # Save as PNG
            output_path = os.path.join(img_dir, filename.replace('.csv', '_speedup.png'))
            fig.write_image(output_path, scale=3)
            print(f"Saved speedup graph to {output_path}")

        except Exception as e:
            print(f"Error processing {filename}: {e}")

if __name__ == "__main__":
    generate_speedup_graphs()