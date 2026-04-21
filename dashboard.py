import streamlit as st
import pandas as pd
import time

st.title("Edge-Processed Weather Station")

placeholder = st.empty()

while True:
    df = pd.read_csv('sensor_log.csv', names=['Time', 'Temp', 'Press', 'Category'], header=0)
    latest = df.iloc[-1]

    with placeholder.container():
        # Display Status with Color
        if latest['Category'] == "Normal":
            st.success(f"STATUS: {latest['Category']}")
        elif latest['Category'] == "Warning":
            st.warning(f"STATUS: {latest['Category']}")
        else:
            st.error(f"STATUS: {latest['Category']}")

        col1, col2 = st.columns(2)
        col1.metric("Temperature", f"{latest['Temp']} °C")
        col2.metric("Pressure", f"{latest['Press']} hPa")

        st.subheader("Temperature Trend")
        st.line_chart(df.set_index('Time')['Temp'])

        st.subheader("Pressure Trend")
        st.line_chart(df.set_index('Time')['Press'])

    time.sleep(5)
